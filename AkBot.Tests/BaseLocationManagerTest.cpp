#include "stdafx.h"
#include "BaseLocationManager.h"
#include "PlainOpponentView.h"
#include "GameImpl.h"
#include "BWAPIOpponentView.h"
#include "BWAPIMapInformation.h"
#include "NullLogger.h"
#include "MapTools.h"
#include "UnitHelper.h"
#include "GameHelper.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace AKBot::Tests;

void discoverMineral(BWAPI::GameData& gameData, int x, int y, int resources)
{
	auto& lastEvent = gameData.events[gameData.eventCount];
	gameData.eventCount++;
	lastEvent.type = BWAPI::EventType::Enum::UnitDiscover;
	lastEvent.v1 = gameData.initialUnitCount;

	auto& mineral = gameData.units[gameData.initialUnitCount];
	mineral.player = gameData.neutral;
	placeMineral(mineral, x, y, resources);
	gameData.initialUnitCount++;
}
void discoverGeyser(BWAPI::GameData& gameData, int x, int y, int resources)
{
	auto& lastEvent = gameData.events[gameData.eventCount];
	gameData.eventCount++;
	lastEvent.type = BWAPI::EventType::Enum::UnitDiscover;
	lastEvent.v1 = gameData.initialUnitCount;

	auto& geyser = gameData.units[gameData.initialUnitCount];
	geyser.player = gameData.neutral;
	placeGeyser(geyser, x, y, resources);
	gameData.initialUnitCount++;
}
void setupEmptyMap(BWAPI::GameData& gameData, int width, int height)
{
	gameData.mapHeight = height;
	gameData.mapWidth = width;
}

namespace AkBotTests
{
	using AKBot::OpponentView;
	using AKBot::PlainOpponentView;
	using AKBot::BWAPIOpponentView;
	using AKBot::BWAPIMapInformation;
	using UAlbertaBot::MapTools;

	TEST_CLASS(BaseLocationManagerTest)
	{
	public:

		TEST_METHOD(CreationOfBaseLocationManagerWithEmptyData)
		{
			auto gameData = std::make_shared<BWAPI::GameData>();
			auto gameImpl = std::make_shared<AKBot::GameImpl>(gameData.get());
			auto game = gameImpl.get();
			auto opponentView = std::make_shared<BWAPIOpponentView>(game);
			UAlbertaBot::BaseLocationManager manager(game, opponentView);
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(CreationOfBaseLocationManagerFromEmptyResources)
			TEST_METHOD_ATTRIBUTE(L"TestCategory", L"WIP")
		END_TEST_METHOD_ATTRIBUTE()
		TEST_METHOD(CreationOfBaseLocationManagerFromEmptyResources)
		{
			auto gameData = std::make_shared<BWAPI::GameData>();
			auto rawGameData = gameData.get();
			auto gameImpl = std::make_shared<AKBot::GameImpl>(rawGameData);
			auto game = gameImpl.get();
			auto opponentView = std::make_shared<BWAPIOpponentView>(BWAPIOpponentView(game));
			UAlbertaBot::BaseLocationManager manager(game, opponentView);
			auto mapInformation = std::make_shared<BWAPIMapInformation>(game);
			auto logger = std::make_shared<NullLogger>();
			auto mapTools = std::make_shared<MapTools>(mapInformation, logger);
			auto baseLocations = std::vector<UAlbertaBot::BaseLocation>();
			
			setP2PForces(rawGameData);
			setPlayers(*rawGameData, 2);
			game->onMatchStart();
			manager.populateBaseLocations(mapTools, baseLocations);
			Assert::AreEqual(0U, baseLocations.size(), L"The base locations list should be empty");
		}

		TEST_METHOD(ClustersWith4MineralsCounted)
		{
			auto gameData = std::make_shared<BWAPI::GameData>();
			auto rawGameData = gameData.get();
			setP2PForces(rawGameData);
			setPlayers(*rawGameData, 2);

			// Create cluster of 4 minerals
			discoverMineral(*rawGameData, 1, 1, 200);
			discoverMineral(*rawGameData, 2, 1, 200);
			discoverMineral(*rawGameData, 3, 1, 200);
			discoverMineral(*rawGameData, 4, 1, 200);
			setupEmptyMap(*rawGameData, 100, 100);

			auto gameImpl = std::make_shared<AKBot::GameImpl>(rawGameData);
			auto game = gameImpl.get();
			BWAPI::BroodwarPtr = game;
			auto opponentView = std::make_shared<BWAPIOpponentView>(BWAPIOpponentView(game));
			UAlbertaBot::BaseLocationManager manager(game, opponentView);
			auto mapInformation = std::make_shared<BWAPIMapInformation>(game);
			auto logger = std::make_shared<NullLogger>();
			auto mapTools = std::make_shared<MapTools>(mapInformation, logger);
			auto baseLocations = std::vector<UAlbertaBot::BaseLocation>();

			game->onMatchStart();
			manager.populateBaseLocations(mapTools, baseLocations);
			Assert::AreEqual(1U, baseLocations.size(), L"The base locations list should be empty");
		}

		TEST_METHOD(ClustersCouldContainMineralsAndGeysers)
		{
			auto gameData = std::make_shared<BWAPI::GameData>();
			auto rawGameData = gameData.get();
			setP2PForces(rawGameData);
			setPlayers(*rawGameData, 2);

			// Create cluster of 4 minerals
			discoverMineral(*rawGameData, 1, 1, 200);
			discoverMineral(*rawGameData, 2, 1, 200);
			discoverMineral(*rawGameData, 3, 1, 200);
			discoverGeyser(*rawGameData, 4, 1, 200);
			setupEmptyMap(*rawGameData, 100, 100);

			auto gameImpl = std::make_shared<AKBot::GameImpl>(rawGameData);
			auto game = gameImpl.get();
			BWAPI::BroodwarPtr = game;
			auto opponentView = std::make_shared<BWAPIOpponentView>(BWAPIOpponentView(game));
			UAlbertaBot::BaseLocationManager manager(game, opponentView);
			auto mapInformation = std::make_shared<BWAPIMapInformation>(game);
			auto logger = std::make_shared<NullLogger>();
			auto mapTools = std::make_shared<MapTools>(mapInformation, logger);
			auto baseLocations = std::vector<UAlbertaBot::BaseLocation>();

			game->onMatchStart();
			manager.populateBaseLocations(mapTools, baseLocations);
			Assert::AreEqual(1U, baseLocations.size(), L"The base locations list should be empty");
		}
	};
}