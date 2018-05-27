#include "ExplorerManager.h"

AKBot::ExplorerManager::ExplorerManager(AKBot::ExplorationCheck explorationCheck)
	: _locations(), _explorationCheck(explorationCheck)
{
}

void AKBot::ExplorerManager::addBaseLocation(const BWAPI::TilePosition & position)
{
	// if we haven't explored it yet
	// When we check just original depot tile position, this could lead to cases when scout reach the point
	// and did not record fact that base is here. It is sometimes happens in the 3 player games.
	// This is very significant error which lead to failure of discovery original enemy base.
	// Possible workaround is create separate rule for checking if base location is explored 
	// by exploring not single depot location, but instead any location near base, to be more confident.
	// This lead to speed loss, but increase reliability.
	addLocation(position);
	addLocation(position + BWAPI::TilePosition(2, 2));
	addLocation(position + BWAPI::TilePosition(-2, 2));
	addLocation(position + BWAPI::TilePosition(2, -2));
	addLocation(position + BWAPI::TilePosition(-2, -2));
}

void AKBot::ExplorerManager::addLocation(const BWAPI::TilePosition & position)
{
	if (_explorationCheck(position)) {
		// Do nothing if we think that location is explored.
		return;
	}

	auto index = std::find(_locations.cbegin(), _locations.cend(), position);
	if (index != _locations.cend())
	{
		// Skip already added location.
		return;
	}

	// Add location to the list of items to explore if location still unexplored.
	_locations.push_back(position);
}

const size_t AKBot::ExplorerManager::locationsToCheckCount() const
{
	return _locations.size();
}

const std::vector<BWAPI::TilePosition>& AKBot::ExplorerManager::locationsToCheck() const
{
	return _locations;
}

void AKBot::ExplorerManager::verifyExpored()
{
	auto eraseMarker = std::remove_if(_locations.begin(), _locations.end(), _explorationCheck);
	_locations.erase(eraseMarker, _locations.end());
}