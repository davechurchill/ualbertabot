#include "ZealotRush.h"
#include "..\..\UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

void AKBot::ZealotRush::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int numZealots = GetAllUnitCount(BWAPI::UnitTypes::Protoss_Zealot);
	int numDragoons = GetAllUnitCount(BWAPI::UnitTypes::Protoss_Dragoon);
	int numNexusAll = GetAllUnitCount(BWAPI::UnitTypes::Protoss_Nexus);
	goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Zealot, numZealots + 8));

	// once we have a 2nd nexus start making dragoons
	if (numNexusAll >= 2)
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Dragoon, numDragoons + 4));
	}
}
