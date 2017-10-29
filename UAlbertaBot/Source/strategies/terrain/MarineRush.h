#pragma once
#include "../StrategyExecutor.h"

namespace AKBot
{
	class MarineRush : public StrategyExecutor
	{
	public:
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}