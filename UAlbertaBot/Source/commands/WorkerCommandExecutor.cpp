#include "WorkerCommandExecutor.h"

namespace {
	void toggle(bool& value) {
		value = !value;
	}
}

AKBot::WorkerCommandExecutor::WorkerCommandExecutor(BotDebugConfiguration& debugConfiguration)
	: _debugConfiguration(debugConfiguration)
{
}

void AKBot::WorkerCommandExecutor::onCommand(const std::string & command)
{
	if (command == "draw resources") {
		toggle(_debugConfiguration.DrawResourceInfo);
	}

	if (command == "draw worker") {
		toggle(_debugConfiguration.DrawWorkerInfo);
	}
}

bool AKBot::WorkerCommandExecutor::isSupported(const std::string & command) const
{
	if (command == "draw resources") {
		return true;
	}

	if (command == "draw worker") {
		return true;
	}

	return false;
}
