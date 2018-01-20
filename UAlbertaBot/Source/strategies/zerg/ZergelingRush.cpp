#include "ZergelingRush.h"
#include "..\..\UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

AKBot::ZergelingRush::ZergelingRush(BWAPI::Player self)
	: _self(self)
{
}

void AKBot::ZergelingRush::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int zerglings = GetAllUnitCount(_self, BWAPI::UnitTypes::Zerg_Zergling);

	goal.push_back(MetaPair(BWAPI::UnitTypes::Zerg_Zergling, zerglings + 6));
}
