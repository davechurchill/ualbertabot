#include "BWAPIMapInformation.h"

AKBot::BWAPIMapInformation::BWAPIMapInformation(BWAPI::Game* game)
	: _game(game)
{
}

int AKBot::BWAPIMapInformation::getWidth() const
{
	return _game->mapWidth();
}

int AKBot::BWAPIMapInformation::getHeight() const
{
	return _game->mapHeight();
}

bool AKBot::BWAPIMapInformation::isWalkable(int x, int y) const
{
	return _game->isWalkable(x, y);
}

bool AKBot::BWAPIMapInformation::isVisible(int x, int y) const
{
	return _game->isVisible(BWAPI::TilePosition(x, y));
}

bool AKBot::BWAPIMapInformation::isBuildable(int tileX, int tileY) const
{
	return _game->isBuildable(BWAPI::TilePosition(tileX, tileY), false);
}

bool AKBot::BWAPIMapInformation::isOccupied(int tileX, int tileY) const
{
	return !_game->isBuildable(BWAPI::TilePosition(tileX, tileY), true)
		&& this->isBuildable(tileX, tileY)
		&& this->isVisible(tileX, tileY);
}

bool AKBot::BWAPIMapInformation::isExplored(int tileX, int tileY) const
{
	return _game->isExplored(tileX, tileY);
}

const BWAPI::Unitset& AKBot::BWAPIMapInformation::getStaticNeutralUnits() const
{
	return _game->getStaticNeutralUnits();
}