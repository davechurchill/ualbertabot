#pragma once
#include "../StrategyExecutor.h"

namespace AKBot
{
	class ZealotRush: public StrategyExecutor
	{
	public:
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}