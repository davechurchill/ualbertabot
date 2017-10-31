#include "MicroCommandExecutor.h"
#include <boost/algorithm/string/predicate.hpp>

namespace {
	void toggle(bool& value) {
		value = !value;
	}
}

AKBot::MicroCommandExecutor::MicroCommandExecutor(BotMicroConfiguration& microConfiguration)
	: _microConfiguration(microConfiguration)
{
}

void AKBot::MicroCommandExecutor::onCommand(const std::string & command)
{
	std::string retreatThresholdPrefix("set retreat threshold ");
	if (boost::starts_with(command, retreatThresholdPrefix)) {
		auto thresholdString = command.substr(retreatThresholdPrefix.length(), command.length() - retreatThresholdPrefix.length());
		_microConfiguration.RetreatThreshold = ::atof(thresholdString.c_str());
	}

	std::string squadUpdatePeriodPrefix("set squad update period ");
	if (boost::starts_with(command, squadUpdatePeriodPrefix)) {
		auto periodString = command.substr(squadUpdatePeriodPrefix.length(), command.length() - squadUpdatePeriodPrefix.length());
		_microConfiguration.SquadUpdateFramePeriod = ::atoi(periodString.c_str());
	}
}

bool AKBot::MicroCommandExecutor::isSupported(const std::string & command) const
{
	if (boost::starts_with(command, "set retreat threshold ")) {
		return true;
	}

	if (boost::starts_with(command, "set squad update period ")) {
		return true;
	}

	return false;
}
