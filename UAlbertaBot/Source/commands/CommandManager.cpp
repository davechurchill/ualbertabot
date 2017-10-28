#include "CommandManager.h"
#include <algorithm>

void AKBot::CommandManager::registerExecutor(std::unique_ptr<CommandExecutor>&& executor)
{
	_executors.push_back(std::move(executor));
}

void AKBot::CommandManager::execute(std::string command)
{
	for (auto&& item : _executors) {
		if (item->isSupported(command)) {
			item->onCommand(command);
			return;
		}
	}
}
