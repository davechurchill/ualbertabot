#include "ZergelingRush.h"
#include "..\..\UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

void AKBot::ZergelingRush::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int zerglings = GetAllUnitCount(BWAPI::UnitTypes::Zerg_Zergling);

	goal.push_back(MetaPair(BWAPI::UnitTypes::Zerg_Zergling, zerglings + 6));
}
