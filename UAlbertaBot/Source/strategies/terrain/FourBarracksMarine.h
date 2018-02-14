#pragma once
#include "../BaseStrategyExecutor.h"
#include <BWAPI\Player.h>

namespace AKBot
{
	class FourBarracksMarine : public BaseStrategyExecutor
	{
		BWAPI::Player _self;
	public:
		FourBarracksMarine(BWAPI::Player self);
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}