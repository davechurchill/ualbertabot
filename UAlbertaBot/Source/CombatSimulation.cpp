#include "CombatSimulation.h"
#include "UnitUtil.h"

using namespace UAlbertaBot;

AKBot::CombatEstimator & UAlbertaBot::CombatSimulation::getCurrentCombatEstimator()
{
	if (_microConfiguration.CombatEstimationStrategy == "fap")
	{
		return _fapCombatEstimator;
	}

	return _sparcraftCombatEstimator;
}

CombatSimulation::CombatSimulation(
	shared_ptr<AKBot::OpponentView> opponentView,
	std::shared_ptr<AKBot::Logger> logger,
	const BotSparCraftConfiguration& sparcraftConfiguration,
	const BotMicroConfiguration& microConfiguration)
	: _sparcraftCombatEstimator(opponentView, logger, sparcraftConfiguration, microConfiguration)
	, _fapCombatEstimator(microConfiguration)
	, _microConfiguration(microConfiguration)
{
}

// sets the starting states based on the combat units within a radius of a given position
// this center will most likely be the position of the forwardmost combat unit we control
bool CombatSimulation::isWinPredicted(
	const std::vector<BWAPI::Unit> ourCombatUnits,
	std::vector<UnitInfo> enemyCombatUnits,
	int currentFrame)
{
	return getCurrentCombatEstimator().isWinPredicted(ourCombatUnits, enemyCombatUnits, currentFrame);
}

void CombatSimulation::printDebugInformation(BWAPI::Position simulationCenter)
{
	auto& estimator = getCurrentCombatEstimator();
	BWAPI::Broodwar->drawCircleMap(simulationCenter.x, simulationCenter.y, 10, BWAPI::Colors::Red, true);
	std::stringstream ss1;
	ss1 << "Initial State:\n";
	ss1 << SparCraft::AITools::StateToStringCompact(_sparcraftCombatEstimator.getSparCraftState()) << "\n\n";

	std::stringstream ss2;

	ss2 << "Predicted Outcome: " << _sparcraftCombatEstimator.getLastScore() << "\n";
	ss2 << SparCraft::AITools::StateToStringCompact(_sparcraftCombatEstimator.getEvaluatedState()) << "\n";

	BWAPI::Broodwar->drawTextScreen(150, 200, "%s", ss1.str().c_str());
	BWAPI::Broodwar->drawTextScreen(300, 200, "%s", ss2.str().c_str());

	BWAPI::Broodwar->drawTextScreen(240, 280, "Combat Sim : %lf", _sparcraftCombatEstimator.getLastScore());
}