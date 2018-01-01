#include "TwoHatchHydralisk.h"
#include "..\..\UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

void AKBot::TwoHatchHydralisk::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int numDrones = GetAllUnitCount(BWAPI::UnitTypes::Zerg_Drone);
	int numHydras = GetAllUnitCount(BWAPI::UnitTypes::Zerg_Hydralisk);

	goal.push_back(MetaPair(BWAPI::UnitTypes::Zerg_Hydralisk, numHydras + 8));
	goal.push_back(MetaPair(BWAPI::UpgradeTypes::Grooved_Spines, 1));
	goal.push_back(MetaPair(BWAPI::UnitTypes::Zerg_Drone, numDrones + 4));
}
