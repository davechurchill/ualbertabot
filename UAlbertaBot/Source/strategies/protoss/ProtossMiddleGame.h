#pragma once
#include "../BaseStrategyExecutor.h"
#include <BWAPI\Player.h>

namespace AKBot
{
	class ProtossMiddleGame : public BaseStrategyExecutor
	{
		BWAPI::Player _self;
	public:
		ProtossMiddleGame(BWAPI::Player self);
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}