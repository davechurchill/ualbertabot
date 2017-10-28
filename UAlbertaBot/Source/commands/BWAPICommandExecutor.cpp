#include "BWAPICommandExecutor.h"

AKBot::BWAPICommandExecutor::BWAPICommandExecutor(BWAPI::Game * game)
	: _game(game)
{
}

void AKBot::BWAPICommandExecutor::onCommand(const std::string & command)
{
	_game->sendText(command.c_str());
}
