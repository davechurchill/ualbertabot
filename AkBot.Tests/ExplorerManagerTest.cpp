#define BOOST_TEST_MODULE ExplorerManagerTest
#include <boost/test/included/unit_test.hpp>
#include "GameImpl.h"
#include "ExplorerManager.h"

using AKBot::ExplorerManager;

template <typename Container>
bool contains(Container const& c, typename Container::const_reference v) {
	return std::find(c.begin(), c.end(), v) != c.end();
}

AKBot::ExplorationCheck alwaysExplored = [](const BWAPI::TilePosition& position) { return true; };
AKBot::ExplorationCheck neverExplored = [](const BWAPI::TilePosition& position) { return false; };

BOOST_AUTO_TEST_CASE(FiveLocationsCheckedForEachBase)
{
	ExplorerManager sut(neverExplored);

	// Add base location to the search element
	sut.addBaseLocation(BWAPI::TilePosition(10, 20));
	// Should be added 5 elements for checking locations
	BOOST_TEST(5U == sut.locationsToCheckCount(), L"5 locations should be checked");
}

BOOST_AUTO_TEST_CASE(AreaNearBaseShouldBeCheckedToIntroduceVariance)
{
	ExplorerManager sut(neverExplored);

	// Add base location to the search element
	sut.addBaseLocation(BWAPI::TilePosition(10, 20));
	// Should be added 5 elements for checking locations
	BOOST_TEST(5U == sut.locationsToCheckCount(), L"5 locations should be checked");
	auto& locations = sut.locationsToCheck();
	BOOST_TEST(contains(locations, BWAPI::TilePosition(10, 20)), L"Base location should be marked for exploration");
}

BOOST_AUTO_TEST_CASE(RegularPointOfInterestAddSingleLocation)
{
	ExplorerManager sut(neverExplored);

	// Add base location to the search element
	sut.addLocation(BWAPI::TilePosition(10, 20));
	// Should be added 5 elements for checking locations
	BOOST_TEST(1U == sut.locationsToCheckCount(), L"1 locations should be explored for regular location");
}

BOOST_AUTO_TEST_CASE(AlreadyExploredLocationsDoesNotAdded)
{
	ExplorerManager sut(alwaysExplored);

	// Add base location to the search element
	sut.addLocation(BWAPI::TilePosition(10, 20));
	// Should be added 5 elements for checking locations
	BOOST_TEST(0U == sut.locationsToCheckCount(), L"1 locations should be explored for regular location");
}

BOOST_AUTO_TEST_CASE(ExploredLocationsRemovedFromTargets)
{
	bool isTargetExplored = false;
	ExplorerManager sut([&isTargetExplored](const BWAPI::TilePosition& position) { return isTargetExplored; });

	// Add base location to the search element
	sut.addLocation(BWAPI::TilePosition(10, 20));
	BOOST_TEST(1U == sut.locationsToCheckCount(), L"1 locations should be not explored before runing verification");

	// Mark location as eplored.
	isTargetExplored = true;
	sut.verifyExpored();
	BOOST_TEST(0U == sut.locationsToCheckCount(), L"The location should be removed if it is explored");
}