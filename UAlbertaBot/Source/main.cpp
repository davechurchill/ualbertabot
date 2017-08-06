/* 
 +----------------------------------------------------------------------+
 | UAlbertaBot                                                          |
 | Author: David Churchill <dave.churchill@gmail.com>                   |
 | https://github.com/davechurchill/ualbertabot                         |
 +----------------------------------------------------------------------+
*/

// BWAPI Libraries
#include <BWAPI.h>
#include <BWAPI/Client.h>

// C++ Libraries
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <boost\di.hpp>

// 3rd Party Libraries
#include "rapidjson\document.h"


// UAlbertaBot Libraries
#include "BotModule.h"
#include "UAlbertaBot_Arena.h"
#include "UAlbertaBot_Tournament.h"
#include "JSONTools.h"
#include "ParseUtils.h"
#include "UnitUtil.h"
#include "BWAPIOpponentView.h"
#include "BWAPIPrintLogger.h"
#include "BWAPIMapInformation.h"

using namespace UAlbertaBot;
namespace di = boost::di;

AKBot::BWAPIOpponentView opponentView;
AKBot::BWAPIPrintLogger logger;

void UAlbertaBot_BWAPIReconnect() 
{
    while(!BWAPI::BWAPIClient.connect())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 }); 
    }
}

template <class, class TScope>
class session_scope {
	class session_guard {
	public:
		explicit session_guard(bool& guard) : guard_(guard) { guard = true; }
		~session_guard() { guard_ = false; }

	private:
		bool& guard_;
	};

public:
	template <class TExpected, class TGiven>
	class scope {
		using scope_type = typename TScope::template scope<TExpected, TGiven>;

	public:
		template <class T>
		using is_referable = typename scope_type::template is_referable<T>;

		template <class T, class TName, class TProvider>
#if defined(__MSVC__)
		static auto try_create(const TProvider& provider)->T;
#else
		static auto try_create(const TProvider& provider) -> decltype(scope_type{}.template try_create<T, TName>(provider));
#endif

		template <class T, class TName, class TProvider>
		auto create(const TProvider& provider) {
			static std::shared_ptr<TGiven> null{ nullptr };
			return get_session() ? scope_.template create<T, TName>(provider) : null;
		}

	private:
		scope_type scope_;
	};

	auto operator()() const { return session_guard{ get_session() }; }

private:
	static auto& get_session() {
		static auto is_in_session = false;
		return is_in_session;
	}
};

template <class TName, class TScope = di::scopes::singleton>
auto session(const TName&, const TScope& = {}) {
	return session_scope<TName, TScope>{};
}
auto my_session = [] {};

class scoped_scope {
public:
	template <class, class T>
	class scope {
	public:
		template <class T_>
		using is_referable = typename di::wrappers::shared<scoped_scope, T>::template is_referable<T_>;

		template <class, class, class TProvider, class T_ = di::aux::decay_t<decltype(di::aux::declval<TProvider>().get())>>
		static decltype(di::wrappers::shared<scoped_scope, T_>{
			std::shared_ptr<T_>{std::shared_ptr<T_>{di::aux::declval<TProvider>().get()}}})
			try_create(const TProvider &);

			template <class T_, class, class TProvider>
			auto create(const TProvider &provider) {
				return create_impl<di::aux::decay_t<decltype(provider.get())>>(provider);
			}

			scope() = default;
			scope(scope &&other) noexcept : object_(other.object_) { other.object_ = nullptr; }
			~scope() noexcept { delete object_; }

	private:
		template <class, class TProvider>
		auto create_impl(const TProvider &provider) {
			if (!object_) {
				object_ = new std::shared_ptr<T>{ provider.get() };
			}
			return di::wrappers::shared<scoped_scope, T, std::shared_ptr<T> &>{*object_};
		}

		std::shared_ptr<T> *object_ = nullptr;
	};
};

static constexpr scoped_scope scoped{};

class BotPlayer
{
	unique_ptr<BotModule> m;
public:
	BotPlayer(unique_ptr<BotModule> _m) : m(std::move(_m)) {}

	bool isValid() const { return !!m.get(); }

	void UAlbertaBot_PlayGame()
	{
		// The main game loop, which continues while we are connected to BWAPI and in a game
		while (BWAPI::BWAPIClient.isConnected() && BWAPI::Broodwar->isInGame())
		{
			// Handle each of the events that happened on this frame of the game
			for (const BWAPI::Event & e : BWAPI::Broodwar->getEvents())
			{
				switch (e.getType())
				{
				case BWAPI::EventType::MatchStart: { m->onStart();                      break; }
				case BWAPI::EventType::MatchEnd: { m->onEnd(e.isWinner());            break; }
				case BWAPI::EventType::MatchFrame: { m->onFrame();                      break; }
				case BWAPI::EventType::UnitShow: { m->onUnitShow(e.getUnit());        break; }
				case BWAPI::EventType::UnitHide: { m->onUnitHide(e.getUnit());        break; }
				case BWAPI::EventType::UnitCreate: { m->onUnitCreate(e.getUnit());      break; }
				case BWAPI::EventType::UnitMorph: { m->onUnitMorph(e.getUnit());       break; }
				case BWAPI::EventType::UnitDestroy: { m->onUnitDestroy(e.getUnit());     break; }
				case BWAPI::EventType::UnitRenegade: { m->onUnitRenegade(e.getUnit());    break; }
				case BWAPI::EventType::UnitComplete: { m->onUnitComplete(e.getUnit());    break; }
				case BWAPI::EventType::SendText: { m->onSendText(e.getText());        break; }
				}
			}

			BWAPI::BWAPIClient.update();
			if (!BWAPI::BWAPIClient.isConnected())
			{
				std::cout << "Disconnected\n";
				break;
			}
		}

		std::cout << "Game Over\n";
	}
};

BotPlayer createBot(const std::string& mode) {
	auto base_bot_module = [] {
		return di::make_injector(
			di::bind<AKBot::Logger>().to<AKBot::BWAPIPrintLogger>(),
			di::bind<AKBot::OpponentView>().to<AKBot::BWAPIOpponentView>(),
			di::bind<AKBot::MapInformation>().to<AKBot::BWAPIMapInformation>()
		);
	};
	auto injector = di::make_injector(
		base_bot_module(),
		/*di::bind<BotModule>().to([&](const auto& injector) -> BotModule&& {
			if (mode == "Tournament") {
				return injector.template create<UAlbertaBot_Tournament&&>();
			}
			else if (mode == "Arena") {
				return injector.template create<UAlbertaBot_Arena&&>();
			}
			else {
				return nullptr;
			}
		}).in(di::unique)*/
		di::bind<BotModule>().to<UAlbertaBot_Tournament>().in(di::unique)
	);
	//injector.create<ScoutManager>();

	// The UAlbertaBot module which will handle all the game logic
	// All Starcraft logic is in this object, when it destructs it's all cleaned up for the next game
	return injector.create<BotPlayer>();
}

int main(int argc, const char * argv[]) 
{
    bool exitIfStarcraftShutdown = true;

    // parse the bot's configuration file, if it is not found or isn't valid, the program will exit
	auto configurationFile = ParseUtils::FindConfigurationLocation(Config::ConfigFile::ConfigFileLocation);
	ParseUtils::ParseConfigFile(configurationFile);

    size_t gameCount = 0;
	while (true)
    {
        // if we are not currently connected to BWAPI, try to reconnect
        if (!BWAPI::BWAPIClient.isConnected())
        {
            UAlbertaBot_BWAPIReconnect();
        }

        // if we have connected to BWAPI
        while (BWAPI::BWAPIClient.isConnected())
        {
            // wait for the game to start until the game starts
            std::cout << "Waiting for game start\n";
	        while (BWAPI::BWAPIClient.isConnected() && !BWAPI::Broodwar->isInGame()) 
            {
		        BWAPI::BWAPIClient.update();
	        }

            // Check to see if Starcraft shut down somehow
            if (BWAPI::BroodwarPtr == nullptr)
            {
                break;
            }

            // If we are successfully in a game, call the module to play the game
			if (BWAPI::Broodwar->isInGame())
			{
				std::cout << "Playing game " << gameCount++ << " on map " << BWAPI::Broodwar->mapFileName() << "\n";

				std::string botMode = Config::BotInfo::BotMode;
				auto m = createBot(botMode);
				if (!m.isValid())
				{
					std::cerr << "Unknown bot module selected: " << botMode << "\n";
					exit(-1);
				}

				m.UAlbertaBot_PlayGame();
                //UAlbertaBot_PlayGame(m);
            }
        }

        if (exitIfStarcraftShutdown && !BWAPI::BWAPIClient.isConnected())
        {
            return 0;
        }
    }

	return 0;
}
