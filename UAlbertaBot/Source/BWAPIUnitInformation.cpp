#include "BWAPIUnitInformation.h"

AKBot::BWAPIUnitInformation::BWAPIUnitInformation(BWAPI::Game* game)
	: _game(game)
{
}

const BWAPI::Unitset& AKBot::BWAPIUnitInformation::getAllUnits() const
{
	return _game->getAllUnits();
}