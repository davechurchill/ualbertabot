#include "ThreeHatchMutalisk.h"
#include "..\..\UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

AKBot::ThreeHatchMutalisk::ThreeHatchMutalisk(BWAPI::Player self): _self(self)
{
}

void AKBot::ThreeHatchMutalisk::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int numDrones = GetAllUnitCount(_self, BWAPI::UnitTypes::Zerg_Drone);
	int numHydras = GetAllUnitCount(_self, BWAPI::UnitTypes::Zerg_Hydralisk);

	goal.push_back(MetaPair(BWAPI::UnitTypes::Zerg_Hydralisk, numHydras + 12));
	goal.push_back(MetaPair(BWAPI::UnitTypes::Zerg_Drone, numDrones + 4));
}
