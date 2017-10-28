#include "UnitCommandExecutor.h"

namespace {
	void toggle(bool& value) {
		value = !value;
	}
}

AKBot::UnitCommandExecutor::UnitCommandExecutor(BotDebugConfiguration& debugConfiguration)
	: _debugConfiguration(debugConfiguration)
{
}

void AKBot::UnitCommandExecutor::onCommand(const std::string & command)
{
	if (command == "draw health") {
		toggle(_debugConfiguration.DrawUnitHealthBars);
	}

	if (command == "draw enemy") {
		toggle(_debugConfiguration.DrawEnemyUnitInfo);
	}
}

bool AKBot::UnitCommandExecutor::isSupported(const std::string & command) const
{
	if (command == "draw health") {
		return true;
	}

	if (command == "draw enemy") {
		return true;
	}

	return false;
}
