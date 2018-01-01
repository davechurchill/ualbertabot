#pragma once
#include "CommandExecutor.h"
#include "..\CombatCommander.h"
#include "..\BotConfiguration.h"

namespace AKBot
{
	class CombatCommanderCommandExecutor : public CommandExecutor
	{
		BotDebugConfiguration& _debugConfiguration;
	public:
		CombatCommanderCommandExecutor(BotDebugConfiguration& debugConfiguration);
		void onCommand(const std::string& command) override;
		bool isSupported(const std::string& command) const override;
	};
}