#pragma once
#include "../StrategyExecutor.h"

namespace AKBot
{
	class ThreeHatchScourge : public StrategyExecutor
	{
	public:
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}