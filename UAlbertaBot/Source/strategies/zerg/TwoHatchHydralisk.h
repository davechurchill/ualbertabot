#pragma once
#include "../StrategyExecutor.h"

namespace AKBot
{
	class TwoHatchHydralisk : public StrategyExecutor
	{
	public:
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}