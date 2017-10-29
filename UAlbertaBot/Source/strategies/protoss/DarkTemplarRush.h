#pragma once
#include "../StrategyExecutor.h"

namespace AKBot
{
	class DarkTemplarRush : public StrategyExecutor
	{
	public:
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}