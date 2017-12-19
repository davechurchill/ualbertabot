#include "CombatCommanderCommandExecutor.h"
#include <boost/algorithm/string/predicate.hpp>

namespace {
	void toggle(bool& value) {
		value = !value;
	}
}

AKBot::CombatCommanderCommandExecutor::CombatCommanderCommandExecutor(BotDebugConfiguration& debugConfiguration)
	: _debugConfiguration(debugConfiguration)
{
}

void AKBot::CombatCommanderCommandExecutor::onCommand(const std::string & command)
{
	std::string toggleCombatManagerTracePrefix("toggle combat manager trace");
	if (boost::starts_with(command, toggleCombatManagerTracePrefix)) {
		toggle(_debugConfiguration.TraceCombatManagerLogic);
	}
}

bool AKBot::CombatCommanderCommandExecutor::isSupported(const std::string & command) const
{
	if (boost::starts_with(command, "toggle combat manager trace")) {
		return true;
	}

	return false;
}
