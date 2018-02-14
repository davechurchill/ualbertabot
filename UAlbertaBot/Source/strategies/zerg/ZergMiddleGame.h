#pragma once
#include "../BaseStrategyExecutor.h"
#include <BWAPI\Player.h>

namespace AKBot
{
	class ZergMiddleGame : public BaseStrategyExecutor
	{
		BWAPI::Player _self;
	public:
		ZergMiddleGame(BWAPI::Player self);
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}