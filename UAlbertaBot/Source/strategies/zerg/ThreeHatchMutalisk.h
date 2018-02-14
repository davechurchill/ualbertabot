#pragma once
#include "../BaseStrategyExecutor.h"
#include <BWAPI\Player.h>

namespace AKBot
{
	class ThreeHatchMutalisk : public BaseStrategyExecutor
	{
		BWAPI::Player _self;
	public:
		ThreeHatchMutalisk(BWAPI::Player self);
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}