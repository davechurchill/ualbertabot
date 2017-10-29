#include "DragoonRush.h"
#include "UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

void AKBot::DragoonRush::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int numDragoons = GetAllUnitCount(BWAPI::UnitTypes::Protoss_Dragoon);

	goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Dragoon, numDragoons + 6));
}
