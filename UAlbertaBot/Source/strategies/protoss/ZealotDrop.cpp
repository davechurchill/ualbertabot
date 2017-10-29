#include "ZealotDrop.h"
#include "UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

void AKBot::ZealotDrop::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int numZealots = GetAllUnitCount(BWAPI::UnitTypes::Protoss_Zealot);

	if (numZealots == 0)
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Zealot, numZealots + 4));
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Shuttle, 1));
	}
	else
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Zealot, numZealots + 8));
	}
}
