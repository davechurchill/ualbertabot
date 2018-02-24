#pragma once

#include "Common.h"
#include "UnitData.h"
#include "OpponentView.h"
#include "UnitInfoManager.h"
#include "Logger.h"
#include "BotConfiguration.h"
#include "simulation\SparCraftCombatEstimator.h"
#include "simulation\FAPCombatEstimator.h"

namespace UAlbertaBot
{
class CombatSimulation
{
	AKBot::SparCraftCombatEstimator _sparcraftCombatEstimator;
	AKBot::FAPCombatEstimator _fapCombatEstimator;

	// Name of the actual combat estimator which will be used for estimation of combats.
	std::string _combatEstimatorName;

	AKBot::CombatEstimator& getCurrentCombatEstimator();
public:

	CombatSimulation(
		shared_ptr<AKBot::OpponentView> opponentView,
		shared_ptr<AKBot::Logger> logger,
		const BotSparCraftConfiguration& sparcraftConfiguration,
		const BotMicroConfiguration& microConfiguration);

	bool isWinPredicted(
		const std::vector<BWAPI::Unit> ourCombatUnits,
		std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits,
		int currentFrame);
	void setEstimator(std::string combatEstimatorName);
	// Gets name of the current combat estimator
	const std::string getEstimator() const { return _combatEstimatorName; }

	void printDebugInformation(BWAPI::Position simulationCenter);
};
}