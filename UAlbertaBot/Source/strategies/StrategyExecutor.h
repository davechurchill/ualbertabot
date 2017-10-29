#pragma once
#include "../Strategy.h"

namespace AKBot
{
	using UAlbertaBot::MetaPairVector;

	class StrategyExecutor
	{
		virtual void getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const = 0;
	};
}