#pragma once
#include "../BaseStrategyExecutor.h"

namespace AKBot
{
	class MarineRush : public BaseStrategyExecutor
	{
	public:
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}