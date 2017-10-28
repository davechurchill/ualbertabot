#include "BaseLocationCommandExecutor.h"

namespace {
	void toggle(bool& value) {
		value = !value;
	}
}

AKBot::BaseLocationCommandExecutor::BaseLocationCommandExecutor(BotDebugConfiguration& debugConfiguration)
	: _debugConfiguration(debugConfiguration)
{
}

void AKBot::BaseLocationCommandExecutor::onCommand(const std::string & command)
{
	if (command == "draw base tiles") {
		toggle(_debugConfiguration.DrawBaseTiles);
	}
}

bool AKBot::BaseLocationCommandExecutor::isSupported(const std::string & command) const
{
	if (command == "draw base tiles") {
		return true;
	}

	return false;
}
