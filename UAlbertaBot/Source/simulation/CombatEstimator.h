#pragma once
#include <BWAPI/Unit.h>
#include "../UnitData.h"

namespace AKBot
{
	class CombatEstimator
	{
	public:
		virtual bool isWinPredicted(
			const std::vector<BWAPI::Unit> ourCombatUnits,
			std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits,
			int currentFrame) = 0;
	};
}