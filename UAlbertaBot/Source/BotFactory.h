#pragma once
#include <string>
#include "BotPlayer.h"
#include "BotConfiguration.h"

namespace AKBot
{
	BotPlayer createBot(const std::string& mode, BotConfiguration& configuration, const std::string& configurationFilePath);
}
