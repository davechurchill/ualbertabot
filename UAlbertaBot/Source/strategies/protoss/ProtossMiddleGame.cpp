#include "ProtossMiddleGame.h"
#include "..\..\UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

void AKBot::ProtossMiddleGame::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int numZealots = GetAllUnitCount(BWAPI::UnitTypes::Protoss_Zealot);
	int numDragoons = GetAllUnitCount(BWAPI::UnitTypes::Protoss_Dragoon);

	goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Zealot, numZealots + 8));
	goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Dragoon, numDragoons + 6));
}
