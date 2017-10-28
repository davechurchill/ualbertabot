#pragma once
#include <vector>
#include <memory>
#include "CommandExecutor.h"

namespace AKBot
{
	class CommandManager
	{
		std::vector<std::unique_ptr<CommandExecutor>> _executors;
	public:
		void registerExecutor(std::unique_ptr<CommandExecutor>&& executor);
		void execute(std::string command);
	};
}