#define BOOST_TEST_MODULE RangedManagerTest
#include <boost/test/included/unit_test.hpp>
#include "GameImpl.h"
#include "RangedManager.h"
#include "BWAPIOpponentView.h"
#include "SupportLib\UnitHelper.h"
#include "SupportLib\GameHelper.h"

using namespace AKBot::Tests;
using UAlbertaBot::RangedManager;
using AKBot::BWAPIOpponentView;
using UAlbertaBot::SquadOrder;
using namespace UAlbertaBot::SquadOrderTypes;

struct BroodwarFixture
{
	~BroodwarFixture() { BWAPI::BroodwarPtr = nullptr; }
};

BOOST_FIXTURE_TEST_CASE(EmptySquadDoesNotGenerateActions, BroodwarFixture)
{
	auto gameData = std::make_shared<BWAPI::GameData>();
	auto gameImpl = std::make_shared<AKBot::GameImpl>(gameData.get());
	auto game = gameImpl.get();
	auto opponentView = std::make_shared<BWAPIOpponentView>(game);
	auto baseLocationManager = std::make_shared<UAlbertaBot::BaseLocationManager>(game, opponentView);
	BotMicroConfiguration microConfiguration;
	RangedManager sut(opponentView, baseLocationManager, microConfiguration);

	// Add base location to the search element
	std::vector<BWAPI::Unit> meleeUnits;
	std::vector<BWAPI::Unit> targets;
	sut.assignTargets(meleeUnits, targets);

	auto const& observation = sut.getObservations();
	// Should be added 5 elements for checking locations
	BOOST_TEST(0U == observation.size(), L"Zero actions generated for empty squad");
}
BOOST_FIXTURE_TEST_CASE(HiddenLurkerIsIgnored, BroodwarFixture)
{
	// This test case is checking that hidden Lurker would not affect
	// results of the game, by blocking the actual targetting logic.
	auto gameData = std::make_shared<BWAPI::GameData>();
	auto rawGameData = gameData.get();

	placeUnit(gameData->units[0], BWAPI::UnitTypes::Zerg_Spire, 100, 100);
	gameData->units[0].player = 1;
	gameData->units[0].isVisible[0] = true;
	placeUnit(gameData->units[1], BWAPI::UnitTypes::Zerg_Lurker, 100, 120);
	gameData->units[1].hitPoints = 0;
	gameData->units[1].shields = 0;
	gameData->units[1].player = 1;
	gameData->units[1].isVisible[0] = true;
	placeUnit(gameData->units[2], BWAPI::UnitTypes::Protoss_Dragoon, 120, 110);
	gameData->units[2].player = 0;
	gameData->units[2].isVisible[0] = true;

	auto gameImpl = std::make_shared<AKBot::GameImpl>(gameData.get());
	auto game = gameImpl.get();
	BWAPI::BroodwarPtr = game;

	setP2PForces(rawGameData);
	setPlayers(*rawGameData, 2);
	auto opponentView = std::make_shared<BWAPIOpponentView>(game);
	auto baseLocationManager = std::make_shared<UAlbertaBot::BaseLocationManager>(game, opponentView);
	BotMicroConfiguration microConfiguration;
	RangedManager sut(opponentView, baseLocationManager, microConfiguration);

	// Add base location to the search element
	std::vector<BWAPI::Unit> rangedUnits;
	std::vector<BWAPI::Unit> targets;

	targets.push_back(game->getUnit(0));
	targets.push_back(game->getUnit(1));
	rangedUnits.push_back(game->getUnit(2));

	game->onMatchStart();

	sut.setCurrentOrder(SquadOrder(Attack, BWAPI::Position(0, 0), 800));
	sut.assignTargets(rangedUnits, targets);

	auto const& observations = sut.getObservations();
	// Should be added 5 elements for checking locations
	BOOST_TEST(1U == observations.size(), L"Zero actions generated for empty squad");
	auto unit = observations.begin()->first;
	auto observation = observations.begin()->second;
	auto attackOrderIssued = observation.shouldAttack || observation.shouldKiteTarget || observation.shouldMutaDance;
	BOOST_TEST(
		attackOrderIssued,
		L"Attack order should be issued");
	BOOST_TEST(
		BWAPI::UnitTypes::Zerg_Spire.getID() == observation.targetUnit->getType().getID(),
		L"Spire should be selected");
}
