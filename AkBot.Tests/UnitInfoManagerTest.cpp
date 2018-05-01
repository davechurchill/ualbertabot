#define BOOST_TEST_MODULE UnitInfoManagerTest
#include <boost/test/included/unit_test.hpp>
#include "GameImpl.h"
#include "UnitInfoManager.h"
#include "BWAPIOpponentView.h"
#include "SupportLib\UnitHelper.h"
#include "SupportLib\GameHelper.h"
#include "SupportLib/GameBuilder.h"
#include "SupportLib/BWAPISession.h"

using namespace AKBot::Tests;
using UAlbertaBot::UnitInfoManager;
using UAlbertaBot::UnitInfo;
using AKBot::BWAPIOpponentView;

BOOST_AUTO_TEST_CASE(InvisibleUnitsDoesNotCountedDuringTesting)
{
	AKBot::Tests::GameBuilder builder;
	builder.setP2PForces()
		.setPlayers(2)
		.setEnemy(1)
		.setupEmptyMap(100, 100);

	auto zergling1 = builder.nextUnit()
		.unit(BWAPI::UnitTypes::Zerg_Zergling)
		.position(100, 100)
		.player(1)
		.playerVisibility(0, true);
	builder.registerUnitDiscoverUnit(zergling1.data().id);
	auto zergling2 = builder.nextUnit()
		.unit(BWAPI::UnitTypes::Zerg_Zergling)
		.position(100, 120)
		.player(1)
		.playerVisibility(0, false);
	builder.registerUnitDiscoverUnit(zergling2.data().id);

	auto game = builder.getGame();
	BWAPISession session(game);

	auto opponentView = std::make_shared<BWAPIOpponentView>(game);
	UnitInfoManager sut(opponentView);
	game->onMatchStart();
	game->onMatchFrame();

	sut.update();

	// Add base location to the search element
	std::vector<UnitInfo> targets;
	sut.getNearbyForce(targets, BWAPI::Position(0, 0), opponentView->defaultEnemy(), 100000);

	// Should be added 0 elements for checking locations
	BOOST_TEST(1U == targets.size(), L"Just single visible unit counted");
}