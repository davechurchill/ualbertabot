#pragma once
#include <BWAPI\Position.h>
#include <vector>
#include <functional>

namespace AKBot
{
	typedef std::function<bool(const BWAPI::TilePosition&)> ExplorationCheck;

	class ExplorerManager
	{
		std::vector<BWAPI::TilePosition> _locations;
		ExplorationCheck _explorationCheck;
	public:
		// Create new isntance of ExplorerManager with function performing check of tiles for exploration status.
		ExplorerManager(ExplorationCheck explorationCheck);

		// Adds base location for exploration by specify position of the base.
		// This like regular location, but we want to add couple more points to check,
		// to account for different base placement models
		void addBaseLocation(const BWAPI::TilePosition& position);
		
		// Add regular location for exploration
		// Difference between regular location and base location is that no base-specific optimization is added
		void addLocation(const BWAPI::TilePosition& position);

		// Gets count of elements which are currenly pending explorations
		const size_t locationsToCheckCount() const;

		// Locations which should be explored.
		const std::vector<BWAPI::TilePosition>& locationsToCheck() const;

		// Verify status of target exploration points.
		// If some of them already explored, remove them from the list.
		void verifyExpored();
	};
}