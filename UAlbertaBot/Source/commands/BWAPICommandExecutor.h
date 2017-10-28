#pragma once
#include "CommandExecutor.h"
#include <BWAPI\Game.h>

namespace AKBot
{
	class BWAPICommandExecutor: public CommandExecutor
	{
		BWAPI::Game* _game;
	public:
		BWAPICommandExecutor(BWAPI::Game* game);
		void onCommand(const std::string& command) override;
	};
}