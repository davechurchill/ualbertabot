#include "ThreeHatchScourge.h"
#include "..\..\UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

AKBot::ThreeHatchScourge::ThreeHatchScourge(BWAPI::Player self): _self(self)
{
}

void AKBot::ThreeHatchScourge::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int numDrones = GetAllUnitCount(_self, BWAPI::UnitTypes::Zerg_Drone);
	int numScourge = GetAllUnitCount(_self, BWAPI::UnitTypes::Zerg_Scourge);
	int numHydras = GetAllUnitCount(_self, BWAPI::UnitTypes::Zerg_Hydralisk);

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
