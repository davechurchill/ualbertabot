#pragma once
#include "../BaseStrategyExecutor.h"
#include <BWAPI\Player.h>

namespace AKBot
{
	class ThreeHatchScourge : public BaseStrategyExecutor
	{
		BWAPI::Player _self;
	public:
		ThreeHatchScourge(BWAPI::Player self);
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}