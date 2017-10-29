
#pragma once
#include "../StrategyExecutor.h"

namespace AKBot
{
	class DragoonRush : public StrategyExecutor
	{
	public:
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}