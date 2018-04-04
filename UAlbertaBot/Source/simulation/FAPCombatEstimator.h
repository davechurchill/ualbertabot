#pragma once
#include "CombatEstimator.h"
#include "FAP.h"
#include "..\BotConfiguration.h"

namespace AKBot
{
	class FAPCombatEstimator : public CombatEstimator
	{
	private:
		UAlbertaBot::FastAPproximation fap;
		const BotMicroConfiguration& _microConfiguration;

		void setCombatUnits(
			const std::vector<BWAPI::Unit> ourCombatUnits,
			std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits,
			int currentFrame);
		double simulateCombat();
	public:
		FAPCombatEstimator(
			const BotMicroConfiguration& microConfiguration);
		bool isWinPredicted(
			const std::vector<BWAPI::Unit>& ourCombatUnits,
			const std::vector<UAlbertaBot::UnitInfo>& enemyCombatUnits,
			int currentFrame) override;
	};
}