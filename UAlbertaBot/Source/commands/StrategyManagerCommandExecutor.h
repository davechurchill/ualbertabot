
#pragma once
#include "CommandExecutor.h"
#include "../StrategyManager.h"

namespace AKBot
{
	using UAlbertaBot::StrategyManager;

	class StrategyManagerCommandExecutor : public CommandExecutor
	{
		StrategyManager& _strategyManager;
	public:
		StrategyManagerCommandExecutor(StrategyManager& strategyManager);
		void onCommand(const std::string& command) override;
		bool isSupported(const std::string& command) const override;
	};
}