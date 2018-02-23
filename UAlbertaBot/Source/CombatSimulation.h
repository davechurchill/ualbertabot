#pragma once

#include "Common.h"

#ifdef USING_VISUALIZATION_LIBRARIES
	#include "Visualizer.h"
#endif

#include "UnitData.h"
#include "OpponentView.h"
#include "UnitInfoManager.h"
#include "Logger.h"
#include "BotConfiguration.h"
#include "simulation\SparCraftCombatEstimator.h"

namespace UAlbertaBot
{
class CombatSimulation
{
	AKBot::SparCraftCombatEstimator _sparcraftCombatEstimator;

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

	void printDebugInformation(BWAPI::Position simulationCenter);
};
}