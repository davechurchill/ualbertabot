#pragma once

#include "Common.h"
#include "UnitData.h"
#include "OpponentView.h"
#include "UnitInfoManager.h"
#include "Logger.h"
#include "BotConfiguration.h"
#include "simulation\SparCraftCombatEstimator.h"
#include "simulation\FAPCombatEstimator.h"

namespace AKBot
{
	struct CombatEstimatorDebugProvider
	{
		virtual void printDebugInformation(BWAPI::Position simulationCenter) = 0;
	};

	struct CombatSimulationEntry
	{
		std::unique_ptr<CombatEstimator> estimator;
		std::unique_ptr<CombatEstimatorDebugProvider> debugProvider;
	};
}

namespace UAlbertaBot
{
class CombatSimulation
{
	const BotMicroConfiguration& _microConfiguration;
	std::map<std::string, AKBot::CombatSimulationEntry> registry;

	AKBot::CombatEstimator& getCurrentCombatEstimator();
	const AKBot::CombatSimulationEntry& getCurrentSimulationEntry();

	// Register custom FAP estimator.
	void registerFAPEstimator(
		const char* name,
		std::function<double(const AKBot::FAPPlayerState&)> playerScoreFunction,
		std::function<double(const AKBot::FAPCombatEstimator&)> scoreFunction);
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

	// Gets name of the current combat estimator
	const std::string getEstimator() const { return _microConfiguration.CombatEstimationStrategy; }

	void printDebugInformation(BWAPI::Position simulationCenter);
};
}