#include "StrategyManagerCommandExecutor.h"
#include <boost/algorithm/string/predicate.hpp>

AKBot::StrategyManagerCommandExecutor::StrategyManagerCommandExecutor(StrategyManager& strategyManager)
	: _strategyManager(strategyManager)
{
}

void AKBot::StrategyManagerCommandExecutor::onCommand(const std::string & command)
{
	std::string prefix("set strategy ");
	auto strategyName = command.substr(prefix.length(), command.length() - prefix.length());
	_strategyManager.setPreferredStrategy(strategyName);
}

bool AKBot::StrategyManagerCommandExecutor::isSupported(const std::string & command) const
{
	if (boost::starts_with(command, "set strategy ")) {
		return true;
	}

	return false;
}
