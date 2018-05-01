#define BOOST_TEST_MODULE ClusterBaseDetectorTest
#include <boost/test/included/unit_test.hpp>
#include "GameImpl.h"
#include "BWAPIOpponentView.h"
#include "BWAPIMapInformation.h"
#include "basedetection/ClusterBaseDetector.h"
#include "../SupportLib/PlainOpponentView.h"
#include "../SupportLib/NullLogger.h"
#include "../SupportLib/UnitHelper.h"
#include "../SupportLib/GameHelper.h"
#include "../SupportLib/GameBuilder.h"
#include "../SupportLib/BWAPISession.h"

using namespace AKBot::Tests;
using namespace AkBotTests;
using AKBot::OpponentView;
using AKBot::PlainOpponentView;
using AKBot::BWAPIOpponentView;
using AKBot::BWAPIMapInformation;
using UAlbertaBot::MapTools;

BOOST_AUTO_TEST_CASE(CreationOfBaseLocationManagerFromEmptyResources)
{
	AKBot::Tests::GameBuilder builder;
	builder.setP2PForces()
		.setPlayers(2)
		.setupEmptyMap(100, 100);

	auto game = builder.getGame();
	BWAPISession session(game);

	auto opponentView = std::make_shared<BWAPIOpponentView>(BWAPIOpponentView(game));
	auto mapInformation = std::make_shared<BWAPIMapInformation>(game);
	auto logger = std::make_shared<NullLogger>();
	auto mapTools = std::make_shared<MapTools>(mapInformation, logger);
	AKBot::ClusterBaseDetector baseDetector(
		game,
		opponentView,
		mapTools);
	auto baseLocations = std::vector<UAlbertaBot::BaseLocation>();

	game->onMatchStart();
	baseDetector.detectBases(baseLocations);
	BOOST_TEST(0U == baseLocations.size(), L"The base locations list should be empty");
}

BOOST_AUTO_TEST_CASE(ClustersWith4MineralsCounted)
{
	AKBot::Tests::GameBuilder builder;
	builder.setP2PForces()
		.setPlayers(2)
		.setupEmptyMap(100, 100);

	// Create cluster of 4 minerals
	builder
		.discoverMineral(1, 1, 200)
		.discoverMineral(2, 1, 200)
		.discoverMineral(3, 1, 200)
		.discoverMineral(4, 1, 200);

	auto game = builder.getGame();
	BWAPISession session(game);

	auto opponentView = std::make_shared<BWAPIOpponentView>(BWAPIOpponentView(game));
	auto mapInformation = std::make_shared<BWAPIMapInformation>(game);
	auto logger = std::make_shared<NullLogger>();
	auto mapTools = std::make_shared<MapTools>(mapInformation, logger);
	AKBot::ClusterBaseDetector baseDetector(
		game,
		opponentView,
		mapTools);
	auto baseLocations = std::vector<UAlbertaBot::BaseLocation>();

	game->onMatchStart();
	baseDetector.detectBases(baseLocations);
	BOOST_TEST(1U == baseLocations.size(), L"The base locations list should be empty");
}

BOOST_AUTO_TEST_CASE(ClustersCouldContainMineralsAndGeysers)
{
	AKBot::Tests::GameBuilder builder;
	builder.setP2PForces()
		.setPlayers(2)
		.setupEmptyMap(100, 100);

	// Create cluster of 4 minerals
	builder
		.discoverMineral(1, 1, 200)
		.discoverMineral(2, 1, 200)
		.discoverMineral(3, 1, 200)
		.discoverGeyser(4, 1, 200);

	auto game = builder.getGame();
	BWAPISession session(game);

	auto opponentView = std::make_shared<BWAPIOpponentView>(BWAPIOpponentView(game));
	auto mapInformation = std::make_shared<BWAPIMapInformation>(game);
	auto logger = std::make_shared<NullLogger>();
	auto mapTools = std::make_shared<MapTools>(mapInformation, logger);
	AKBot::ClusterBaseDetector baseDetector(
		game,
		opponentView,
		mapTools);
	auto baseLocations = std::vector<UAlbertaBot::BaseLocation>();

	game->onMatchStart();
	baseDetector.detectBases(baseLocations);
	BOOST_TEST(1U == baseLocations.size(), L"The base locations list should be empty");
}