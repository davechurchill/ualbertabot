#pragma once
#include "../BaseStrategyExecutor.h"

namespace AKBot
{
	class ZergelingRush : public BaseStrategyExecutor
	{
	public:
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}