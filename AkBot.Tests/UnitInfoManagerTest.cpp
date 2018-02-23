#include "stdafx.h"
#include "GameImpl.h"
#include "UnitInfoManager.h"
#include "BWAPIOpponentView.h"
#include "UnitHelper.h"
#include "GameHelper.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AKBot::Tests;

namespace AkBotTests
{
	using UAlbertaBot::UnitInfoManager;
	using UAlbertaBot::UnitInfo;
	using AKBot::BWAPIOpponentView;

	TEST_CLASS(UnitInfoManagerTest)
	{
	public:
		UnitInfoManagerTest()
		{
		}
		~UnitInfoManagerTest()
		{
			BWAPI::BroodwarPtr = nullptr;
		}
		TEST_METHOD(InvisibleUnitsDoesNotCountedDuringTesting)
		{
			auto gameData = std::make_shared<BWAPI::GameData>();
			auto rawGameData = gameData.get();
			
			placeZergZergeling(gameData->units[0], 100, 100);
			gameData->units[0].player = 1;
			gameData->units[0].isVisible[0] = true;
			gameData->events[0].type = BWAPI::EventType::UnitDiscover;
			gameData->events[0].v1 = 0;
			gameData->eventCount++;
			placeZergZergeling(gameData->units[1], 100, 120);
			gameData->units[1].player = 1;
			gameData->units[1].isVisible[0] = false;
			gameData->events[1].type = BWAPI::EventType::UnitDiscover;
			gameData->events[1].v1 = 1;
			gameData->eventCount++;

			setP2PForces(rawGameData);
			rawGameData->self = 0;
			rawGameData->enemy = 1;
			setPlayers(*rawGameData, 2);

			auto gameImpl = std::make_shared<AKBot::GameImpl>(gameData.get());
			auto game = gameImpl.get();
			BWAPI::BroodwarPtr = game;
			auto opponentView = std::make_shared<BWAPIOpponentView>(game);
			UnitInfoManager sut(opponentView);
			game->onMatchStart();
			game->onMatchFrame();
			
			sut.update();

			// Add base location to the search element
			std::vector<UnitInfo> targets;
			sut.getNearbyForce(targets, BWAPI::Position(0,0), opponentView->defaultEnemy(), 100000);

			// Should be added 0 elements for checking locations
			Assert::AreEqual(1U, targets.size(), L"Just single visible unit counted");
		}
	};
}
