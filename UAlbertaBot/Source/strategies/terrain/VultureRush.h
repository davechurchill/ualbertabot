#pragma once
#include "../BaseStrategyExecutor.h"

namespace AKBot
{
	class VultureRush : public BaseStrategyExecutor
	{
	public:
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}