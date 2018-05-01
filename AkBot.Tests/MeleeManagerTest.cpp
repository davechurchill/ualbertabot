#define BOOST_TEST_MODULE MeleeManagerTest
#include <boost/test/included/unit_test.hpp>
#include "GameImpl.h"
#include "MeleeManager.h"
#include "BWAPIOpponentView.h"
#include "SupportLib\UnitHelper.h"
#include "SupportLib\GameHelper.h"
#include "SupportLib/GameBuilder.h"
#include "SupportLib/BWAPISession.h"

using namespace AKBot::Tests;
using UAlbertaBot::MeleeManager;
using AKBot::BWAPIOpponentView;
using UAlbertaBot::SquadOrder;
using namespace UAlbertaBot::SquadOrderTypes;

struct BroodwarFixture
{
	~BroodwarFixture() { BWAPI::BroodwarPtr = nullptr; }
};
BOOST_AUTO_TEST_CASE(EmptySquadDoesNotGenerateActions)
{
	AKBot::Tests::GameBuilder builder;
	auto game = builder.getGame();

	auto opponentView = std::make_shared<BWAPIOpponentView>(game);
	BotBaseDetectionConfiguration baseDetectionConfiguration;
	auto baseLocationManager = std::make_shared<UAlbertaBot::BaseLocationManager>(game, opponentView, baseDetectionConfiguration);
	BotMicroConfiguration microConfiguration;
	MeleeManager sut(opponentView, baseLocationManager, microConfiguration);

	// Add base location to the search element
	std::vector<BWAPI::Unit> meleeUnits;
	std::vector<BWAPI::Unit> targets;
	sut.assignTargets(meleeUnits, targets);

	auto const& observation = sut.getObservations();
	// Should be added 5 elements for checking locations
	BOOST_TEST(0U == observation.size(), L"Zero actions generated for empty squad");
}
BOOST_FIXTURE_TEST_CASE(PreferBunkerOverMedic, BroodwarFixture)
{
	AKBot::Tests::GameBuilder builder;
	builder.setP2PForces()
		.setPlayers(2);

	auto bunker = builder.nextUnit()
		.unit(BWAPI::UnitTypes::Terran_Bunker)
		.position(100, 100)
		.player(1)
		.playerVisibility(0, true);
	auto medic = builder.nextUnit()
		.unit(BWAPI::UnitTypes::Terran_Medic)
		.position(100, 120)
		.player(1)
		.playerVisibility(0, true);
	auto ultralisk = builder.nextUnit()
		.unit(BWAPI::UnitTypes::Zerg_Ultralisk)
		.position(120, 110)
		.player(0)
		.playerVisibility(0, true);

	auto game = builder.getGame();
	BWAPISession session(game);

	game->onMatchStart();

	auto opponentView = std::make_shared<BWAPIOpponentView>(game);
	BotBaseDetectionConfiguration baseDetectionConfiguration;
	auto baseLocationManager = std::make_shared<UAlbertaBot::BaseLocationManager>(game, opponentView, baseDetectionConfiguration);
	BotMicroConfiguration microConfiguration;
	MeleeManager sut(opponentView, baseLocationManager, microConfiguration);

	// Add base location to the search element
	std::vector<BWAPI::Unit> meleeUnits;
	std::vector<BWAPI::Unit> targets;

	targets.push_back(game->getUnit(0));
	targets.push_back(game->getUnit(1));
	meleeUnits.push_back(game->getUnit(2));

	sut.setCurrentOrder(SquadOrder(Attack, BWAPI::Position(0, 0), 800));
	sut.assignTargets(meleeUnits, targets);

	auto const& observations = sut.getObservations();
	// Should be added 5 elements for checking locations
	BOOST_TEST(1U == observations.size(), L"Zero actions generated for empty squad");
	auto unit = observations.begin()->first;
	auto observation = observations.begin()->second;
	BOOST_TEST(
		observation.shouldAttack,
		L"Attack order should be issued");
	BOOST_TEST(
		BWAPI::UnitTypes::Terran_Bunker.getID() == observation.targetUnit->getType().getID(),
		L"Bunker should be selected");
}
BOOST_FIXTURE_TEST_CASE(HiddenLurkerIsIgnored, BroodwarFixture)
{
	// This test case is checking that hidden Lurker would not affect
	// results of the game, by blocking the actual targetting logic.
	AKBot::Tests::GameBuilder builder;
	builder.setP2PForces()
		.setPlayers(2);

	auto spire = builder.nextUnit()
		.unit(BWAPI::UnitTypes::Zerg_Spire)
		.position(100, 100)
		.player(1)
		.playerVisibility(0, true);
	auto lurker = builder.nextUnit()
		.unit(BWAPI::UnitTypes::Zerg_Lurker)
		.position(100, 120)
		.player(1)
		.playerVisibility(0, true)
		.hidden();
	auto dragoon = builder.nextUnit()
		.unit(BWAPI::UnitTypes::Protoss_Dragoon)
		.position(120, 110)
		.player(0)
		.playerVisibility(0, true);

	auto game = builder.getGame();
	BWAPISession session(game);

	game->onMatchStart();
	auto opponentView = std::make_shared<BWAPIOpponentView>(game);
	BotBaseDetectionConfiguration baseDetectionConfiguration;
	auto baseLocationManager = std::make_shared<UAlbertaBot::BaseLocationManager>(game, opponentView, baseDetectionConfiguration);
	BotMicroConfiguration microConfiguration;
	MeleeManager sut(opponentView, baseLocationManager, microConfiguration);

	// Add base location to the search element
	std::vector<BWAPI::Unit> meleeUnits;
	std::vector<BWAPI::Unit> targets;

	targets.push_back(game->getUnit(0));
	targets.push_back(game->getUnit(1));
	meleeUnits.push_back(game->getUnit(2));

	sut.setCurrentOrder(SquadOrder(Attack, BWAPI::Position(0, 0), 800));
	sut.assignTargets(meleeUnits, targets);

	auto const& observations = sut.getObservations();
	// Should be added 5 elements for checking locations
	BOOST_TEST(1U == observations.size(), L"Zero actions generated for empty squad");
	auto unit = observations.begin()->first;
	auto observation = observations.begin()->second;
	BOOST_TEST(
		observation.shouldAttack,
		L"Attack order should be issued");
	BOOST_TEST(
		BWAPI::UnitTypes::Zerg_Spire.getID() == observation.targetUnit->getType().getID(),
		L"Spire should be selected");
}