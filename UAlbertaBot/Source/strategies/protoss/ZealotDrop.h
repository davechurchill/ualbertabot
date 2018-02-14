#pragma once
#include "../BaseStrategyExecutor.h"
#include <BWAPI\Player.h>

namespace AKBot
{
	class ZealotDrop: public BaseStrategyExecutor
	{
		BWAPI::Player _self;
	public:
		ZealotDrop(BWAPI::Player self);
		void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const final;
	};
}