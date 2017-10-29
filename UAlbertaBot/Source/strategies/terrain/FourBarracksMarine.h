#pragma once
#include "../StrategyExecutor.h"

namespace AKBot
{
	class FourBarracksMarine : public StrategyExecutor
	{
	public:
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}