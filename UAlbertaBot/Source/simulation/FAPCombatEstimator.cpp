#include "FAPCombatEstimator.h"
#include "..\UnitUtil.h"

using namespace AKBot;
using namespace UAlbertaBot;

AKBot::FAPCombatEstimator::FAPCombatEstimator(const BotMicroConfiguration & microConfiguration)
	: _microConfiguration(microConfiguration)
{
}

bool FAPCombatEstimator::isWinPredicted(
	const std::vector<BWAPI::Unit>& ourCombatUnits,
	const std::vector<UAlbertaBot::UnitInfo>& enemyCombatUnits,
	int currentFrame)
{
	setCombatUnits(ourCombatUnits, enemyCombatUnits, currentFrame);
	auto score = simulateCombat();
	return score > _microConfiguration.RetreatThreshold;
}


// sets the starting states based on the combat units within a radius of a given position
// this center will most likely be the position of the forwardmost combat unit we control
void FAPCombatEstimator::setCombatUnits(
	const std::vector<BWAPI::Unit> ourCombatUnits,
	std::vector<UnitInfo> enemyCombatUnits,
	int currentFrame)
{
	for (auto & unit : ourCombatUnits)
	{
		if (unit->getType().isWorker() || unit->getHitPoints() == 0 || unit->getType().isBuilding())
		{
			continue;
		}

		if (UnitUtil::IsCombatUnit(unit))
		{
			fap.addIfCombatUnitPlayer1(unit);
		}
	}

	for (UnitInfo & ui : enemyCombatUnits)
	{
		if (ui.type.isWorker() || ui.lastHealth == 0 || ui.type == BWAPI::UnitTypes::Unknown)
		{
			continue;
		}

		if (!ui.type.isFlyer() && ui.completed)
		{
			fap.addIfCombatUnitPlayer2(ui);
		}
	}
}

double AKBot::FAPCombatEstimator::simulateCombat()
{
	fap.simulate(_microConfiguration.CombatEstimationDepth);
	std::pair<int, int> scores = fap.playerScores();

	int score = scores.first - scores.second;
	return double(score);
}