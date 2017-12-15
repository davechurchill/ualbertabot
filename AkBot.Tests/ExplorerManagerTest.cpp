#include "stdafx.h"
#include "GameImpl.h"
#include "ExplorerManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

template <typename Container>
bool contains(Container const& c, typename Container::const_reference v) {
	return std::find(c.begin(), c.end(), v) != c.end();
}

AKBot::ExplorationCheck alwaysExplored = [](const BWAPI::TilePosition& position) { return true; };
AKBot::ExplorationCheck neverExplored = [](const BWAPI::TilePosition& position) { return false; };

namespace AkBotTests
{
	using AKBot::ExplorerManager;

	TEST_CLASS(ExplorerManagerTest)
	{
	public:

		TEST_METHOD(FiveLocationsCheckedForEachBase)
		{
			ExplorerManager sut(neverExplored);

			// Add base location to the search element
			sut.addBaseLocation(BWAPI::TilePosition(10, 20));
			// Should be added 5 elements for checking locations
			Assert::AreEqual(5U, sut.locationsToCheckCount(), L"5 locations should be checked");
		}

		TEST_METHOD(AreaNearBaseShouldBeCheckedToIntroduceVariance)
		{
			ExplorerManager sut(neverExplored);

			// Add base location to the search element
			sut.addBaseLocation(BWAPI::TilePosition(10, 20));
			// Should be added 5 elements for checking locations
			Assert::AreEqual(5U, sut.locationsToCheckCount(), L"5 locations should be checked");
			auto& locations = sut.locationsToCheck();
			Assert::IsTrue(contains(locations, BWAPI::TilePosition(10, 20)), L"Base location should be marked for exploration");
		}

		TEST_METHOD(RegularPointOfInterestAddSingleLocation)
		{
			ExplorerManager sut(neverExplored);

			// Add base location to the search element
			sut.addLocation(BWAPI::TilePosition(10, 20));
			// Should be added 5 elements for checking locations
			Assert::AreEqual(1U, sut.locationsToCheckCount(), L"1 locations should be explored for regular location");
		}

		TEST_METHOD(AlreadyExploredLocationsDoesNotAdded)
		{
			ExplorerManager sut(alwaysExplored);

			// Add base location to the search element
			sut.addLocation(BWAPI::TilePosition(10, 20));
			// Should be added 5 elements for checking locations
			Assert::AreEqual(0U, sut.locationsToCheckCount(), L"1 locations should be explored for regular location");
		}

		TEST_METHOD(ExploredLocationsRemovedFromTargets)
		{
			bool isTargetExplored = false;
			ExplorerManager sut([&isTargetExplored](const BWAPI::TilePosition& position) { return isTargetExplored; });

			// Add base location to the search element
			sut.addLocation(BWAPI::TilePosition(10, 20));
			Assert::AreEqual(1U, sut.locationsToCheckCount(), L"1 locations should be not explored before runing verification");
			
			// Mark location as eplored.
			isTargetExplored = true;
			sut.verifyExpored();
			Assert::AreEqual(0U, sut.locationsToCheckCount(), L"The location should be removed if it is explored");
		}
	};
}
