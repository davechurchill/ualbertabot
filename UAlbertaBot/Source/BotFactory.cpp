#include "BotFactory.h"
#include <boost\di.hpp>
#include "UAlbertaBot_Arena.h"
#include "UAlbertaBot_Tournament.h"
#include "BWAPIOpponentView.h"
#include "debug\BWAPIPrintLogger.h"
#include "BWAPIMapInformation.h"

namespace di = boost::di;
using namespace AKBot;
using namespace UAlbertaBot;

BotPlayer AKBot::createBot(const std::string& mode) {
	auto injectorFactory = [&mode]() -> di::injector<BotPlayer> {
		auto base_bot_module = [] {
			return di::make_injector(
				di::bind<AKBot::Logger>().to<AKBot::BWAPIPrintLogger>(),
				di::bind<AKBot::OpponentView>().to<AKBot::BWAPIOpponentView>(),
				di::bind<AKBot::MapInformation>().to<AKBot::BWAPIMapInformation>()
			);
		};
		if (mode == "Tournament") {
			return di::make_injector(
				base_bot_module(),
				di::bind<BotModule>().to<UAlbertaBot_Tournament>().in(di::unique)
			);
		}
		else if (mode == "Arena") {
			return di::make_injector(
				base_bot_module(),
				di::bind<BotModule>().to<UAlbertaBot_Arena>().in(di::unique)
			);
		}
		else {
			return di::make_injector(
				base_bot_module(),
				di::bind<BotModule>().to([&](const auto& injector)-> std::unique_ptr<BotModule> {
					return nullptr;
				})
			);
		}
	};

	// The UAlbertaBot module which will handle all the game logic
	// All Starcraft logic is in this object, when it destructs it's all cleaned up for the next game
	auto injector = injectorFactory();
	return injector.create<BotPlayer>();
}