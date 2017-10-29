#pragma once
#include "../StrategyExecutor.h"

namespace AKBot
{
	class VultureRush : public StrategyExecutor
	{
	public:
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}