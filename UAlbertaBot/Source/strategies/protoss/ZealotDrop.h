#pragma once
#include "../StrategyExecutor.h"

namespace AKBot
{
	class ZealotDrop: public StrategyExecutor
	{
	public:
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}