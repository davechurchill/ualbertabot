#include "BWAPIUnitInformation.h"

AKBot::BWAPIUnitInformation::BWAPIUnitInformation(BWAPI::Game* game)
	: _game(game)
{
}

const BWAPI::Unitset& AKBot::BWAPIUnitInformation::getAllUnits() const
{
	return _game->getAllUnits();
}

const BWAPI::Unitset AKBot::BWAPIUnitInformation::getUnitsOnTile(int tileX, int tileY) const
{
	return _game->getUnitsOnTile(tileX, tileY);
}