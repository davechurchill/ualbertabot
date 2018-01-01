#pragma once
#include "CommandExecutor.h"
#include "..\BotConfiguration.h"

namespace AKBot
{
	class MicroCommandExecutor : public CommandExecutor
	{
		BotMicroConfiguration& _microConfiguration;
	public:
		MicroCommandExecutor(BotMicroConfiguration& microConfiguration);
		void onCommand(const std::string& command) override;
		bool isSupported(const std::string& command) const override;
	};
}