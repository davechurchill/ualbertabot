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

	std::string squadMoveThresholdPrefix("set squad move threshold ");
	if (boost::starts_with(command, squadMoveThresholdPrefix)) {
		auto thresholdString = command.substr(squadMoveThresholdPrefix.length(), command.length() - squadMoveThresholdPrefix.length());
		_microConfiguration.MoveTargetThreshold = ::atoi(thresholdString.c_str());
	}

	std::string combatEstimationPrefix("set combat estimation ");
	if (boost::starts_with(command, combatEstimationPrefix)) {
		auto combatEstimationString = command.substr(combatEstimationPrefix.length(), command.length() - combatEstimationPrefix.length());
		_microConfiguration.CombatEstimationStrategy = combatEstimationString;
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

	if (boost::starts_with(command, "set squad move threshold ")) {
		return true;
	}

	if (boost::starts_with(command, "set combat estimation ")) {
		return true;
	}

	return false;
}
