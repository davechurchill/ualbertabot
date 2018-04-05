#include "BWAPIHUDInfo.h"

AKBot::BWAPIHUDInfo::BWAPIHUDInfo(BWAPI::Game * game)
	: _game(game)
{
}

BWAPI::Position AKBot::BWAPIHUDInfo::getMousePosition() const
{
	return _game->getMousePosition();
}

BWAPI::Position AKBot::BWAPIHUDInfo::getScreenPosition() const
{
	return _game->getScreenPosition();
}

bool AKBot::BWAPIHUDInfo::getMouseState(BWAPI::MouseButton button) const
{
	return _game->getMouseState(button);
}
