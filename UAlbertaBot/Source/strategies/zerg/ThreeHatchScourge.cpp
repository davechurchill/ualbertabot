#include "ThreeHatchScourge.h"
#include "..\..\UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

void AKBot::ThreeHatchScourge::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int numDrones = GetAllUnitCount(BWAPI::UnitTypes::Zerg_Drone);
	int numScourge = GetAllUnitCount(BWAPI::UnitTypes::Zerg_Scourge);
	int numHydras = GetAllUnitCount(BWAPI::UnitTypes::Zerg_Hydralisk);

	if (numScourge > 40)
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Zerg_Hydralisk, numHydras + 12));
	}
	else
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Zerg_Scourge, numScourge + 12));
	}


	goal.push_back(MetaPair(BWAPI::UnitTypes::Zerg_Drone, numDrones + 4));
}
