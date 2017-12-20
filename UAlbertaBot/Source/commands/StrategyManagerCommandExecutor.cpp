#include "StrategyManagerCommandExecutor.h"
#include <boost/algorithm/string/predicate.hpp>

AKBot::StrategyManagerCommandExecutor::StrategyManagerCommandExecutor(
	StrategyManager& strategyManager,
	BotStrategyConfiguration& strategyConfiguration)
	: _strategyManager(strategyManager)
	, _strategyConfiguration(strategyConfiguration)
{
}

void AKBot::StrategyManagerCommandExecutor::onCommand(const std::string & command)
{
	std::string setStrategyPrefix("set strategy ");
	if (boost::starts_with(command, setStrategyPrefix)) {
		auto strategyName = command.substr(setStrategyPrefix.length(), command.length() - setStrategyPrefix.length());
		_strategyManager.setPreferredStrategy(strategyName);
	}

	std::string setTerranSwitchTimePrefix("set terran switch time ");
	if (boost::starts_with(command, setTerranSwitchTimePrefix)) {
		auto terranSwitchTimeString = command.substr(setTerranSwitchTimePrefix.length(), command.length() - setTerranSwitchTimePrefix.length());
		_strategyConfiguration.TerranStrategySwitchTime = ::atof(terranSwitchTimeString.c_str());
	}

	std::string setZergSwitchTimePrefix("set zerg switch time ");
	if (boost::starts_with(command, setZergSwitchTimePrefix)) {
		auto zergSwitchTimeString = command.substr(setZergSwitchTimePrefix.length(), command.length() - setZergSwitchTimePrefix.length());
		_strategyConfiguration.ZergStrategySwitchTime = ::atof(setZergSwitchTimePrefix.c_str());
	}

	std::string setProtossSwitchTimePrefix("set protoss switch time ");
	if (boost::starts_with(command, setProtossSwitchTimePrefix)) {
		auto protossSwitchTimeString = command.substr(setProtossSwitchTimePrefix.length(), command.length() - setProtossSwitchTimePrefix.length());
		_strategyConfiguration.ProtossStrategySwitchTime = ::atof(protossSwitchTimeString.c_str());
	}
}

bool AKBot::StrategyManagerCommandExecutor::isSupported(const std::string & command) const
{
	if (boost::starts_with(command, "set strategy ")) {
		return true;
	}

	if (boost::starts_with(command, "set terran switch time ")) {
		return true;
	}

	if (boost::starts_with(command, "set zerg switch time ")) {
		return true;
	}

	if (boost::starts_with(command, "set protoss switch time ")) {
		return true;
	}

	return false;
}
