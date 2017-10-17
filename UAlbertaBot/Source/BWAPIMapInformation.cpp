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

bool AKBot::BWAPIMapInformation::isBuildable(int x, int y) const
{
	return _game->isBuildable(BWAPI::TilePosition(x, y), false);
}

bool AKBot::BWAPIMapInformation::isExplored(int tileX, int tileY) const
{
	return _game->isExplored(tileX, tileY);
}

const BWAPI::Unitset& AKBot::BWAPIMapInformation::getStaticNeutralUnits() const
{
	return _game->getStaticNeutralUnits();
}