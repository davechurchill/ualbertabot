#pragma once
#include "CommandExecutor.h"
#include "..\BotConfiguration.h"

namespace AKBot
{
	class UnitCommandExecutor : public CommandExecutor
	{
		BotDebugConfiguration& _debugConfiguration;
	public:
		UnitCommandExecutor(BotDebugConfiguration& debugConfiguration);
		void onCommand(const std::string& command) override;
		bool isSupported(const std::string& command) const override;
	};
}