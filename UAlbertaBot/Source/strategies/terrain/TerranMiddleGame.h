
#pragma once
#include "../BaseStrategyExecutor.h"

namespace AKBot
{
	class TerranMiddleGame : public BaseStrategyExecutor
	{
	public:
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}