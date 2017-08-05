#include "BWAPIMapInformation.h"
#include "BWAPI\Game.h"

int AKBot::BWAPIMapInformation::getWidth() const
{
	return BWAPI::Broodwar->mapWidth();
}

int AKBot::BWAPIMapInformation::getHeight() const
{
	return BWAPI::Broodwar->mapHeight();
}

bool AKBot::BWAPIMapInformation::isWalkable(int x, int y) const
{
	return BWAPI::Broodwar->isWalkable(x, y);
}

bool AKBot::BWAPIMapInformation::isVisible(int x, int y) const
{
	return BWAPI::Broodwar->isVisible(BWAPI::TilePosition(x, y));
}

bool AKBot::BWAPIMapInformation::isBuildable(int x, int y) const
{
	return BWAPI::Broodwar->isBuildable(BWAPI::TilePosition(x, y), false);
}
