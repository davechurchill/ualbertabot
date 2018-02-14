#pragma once
#include "../BaseStrategyExecutor.h"
#include <BWAPI\Player.h>

namespace AKBot
{
	class DarkTemplarRush : public BaseStrategyExecutor
	{
		BWAPI::Player _self;
	public:
		DarkTemplarRush(BWAPI::Player self);
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}