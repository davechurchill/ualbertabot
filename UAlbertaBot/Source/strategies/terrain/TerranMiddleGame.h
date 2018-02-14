#pragma once
#include "../BaseStrategyExecutor.h"
#include <BWAPI\Player.h>

namespace AKBot
{
	class TerranMiddleGame : public BaseStrategyExecutor
	{
		BWAPI::Player _self;
	public:
		TerranMiddleGame(BWAPI::Player self);
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}