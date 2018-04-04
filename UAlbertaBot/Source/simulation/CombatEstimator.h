#pragma once
#include <BWAPI/Unit.h>
#include "../UnitData.h"

namespace AKBot
{
	/* Abstraction for combat estimation algorithms */
	class CombatEstimator
	{
	public:
		/*
		 * Verifies that win predicted for specific combat units.
		 * ourCombatUnits Combat units of current player side.
		 * enemyCombatUnits Collection of units from opponents side
		 * currentFrame Number of frame when battle is started.
		 * Returns true is our side would win. false otherwise.
		 */
		virtual bool isWinPredicted(
			const std::vector<BWAPI::Unit>& ourCombatUnits,
			const std::vector<UAlbertaBot::UnitInfo>& enemyCombatUnits,
			int currentFrame) = 0;
	};
}