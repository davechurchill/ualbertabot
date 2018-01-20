#pragma once
#include "../BaseStrategyExecutor.h"
#include <BWAPI\Player.h>

namespace AKBot
{
	class TwoHatchHydralisk : public BaseStrategyExecutor
	{
		BWAPI::Player _self;
	public:
		TwoHatchHydralisk(BWAPI::Player self);
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}