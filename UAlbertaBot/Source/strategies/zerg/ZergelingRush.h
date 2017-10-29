#pragma once
#include "../StrategyExecutor.h"

namespace AKBot
{
	class ZergelingRush : public StrategyExecutor
	{
	public:
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}