#pragma once
#include <string>
#include <BWAPI/Game.h>
#include "BotPlayer.h"
#include "BotConfiguration.h"

namespace AKBot
{
	BotPlayer createBot(BWAPI::Game* game, const std::string& mode, BotConfiguration& configuration, const std::string& configurationFilePath);
}
