#include "ProductionCommandExecutor.h"

namespace {
	void toggle(bool& value) {
		value = !value;
	}
}

AKBot::ProductionCommandExecutor::ProductionCommandExecutor(BotDebugConfiguration& debugConfiguration)
	: _debugConfiguration(debugConfiguration)
{
}

void AKBot::ProductionCommandExecutor::onCommand(const std::string & command)
{
	if (command == "draw boss search") {
		toggle(_debugConfiguration.DrawBuildOrderSearchInfo);
	}

	if (command == "draw production") {
		toggle(_debugConfiguration.DrawProductionInfo);
	}

	if (command == "draw building") {
		toggle(_debugConfiguration.DrawBuildingInfo);
	}

	if (command == "draw boss state") {
		toggle(_debugConfiguration.DrawBOSSStateInfo);
	}

	if (command == "draw reserved tiles") {
		toggle(_debugConfiguration.DrawReservedBuildingTiles);
	}
}

bool AKBot::ProductionCommandExecutor::isSupported(const std::string & command) const
{
	if (command == "draw boss search") {
		return true;
	}

	if (command == "draw production") {
		return true;
	}

	if (command == "draw building") {
		return true;
	}

	if (command == "draw boss state") {
		return true;
	}

	if (command == "draw reserved tiles") {
		return true;
	}

	return false;
}
