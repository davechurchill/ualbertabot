#pragma once
#include "../StrategyConfiguration.h"

namespace AKBot
{
	using UAlbertaBot::MetaPairVector;
	using UAlbertaBot::BuildOrder;

	class StrategyExecutor
	{
	public:
		/*
		* Create current build order goal
		*/
		virtual void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const = 0;
		virtual void loadOpeningBuildOrder(const BuildOrder& buildOrder) = 0;
		virtual const BuildOrder& getOpeningBuildOrder() const = 0;
	};
}