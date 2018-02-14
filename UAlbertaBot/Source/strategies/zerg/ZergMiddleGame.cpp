#include "ZergMiddleGame.h"
#include "..\..\UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

AKBot::ZergMiddleGame::ZergMiddleGame(BWAPI::Player self) : _self(self)
{
}

void AKBot::ZergMiddleGame::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int zerglings = GetAllUnitCount(_self, BWAPI::UnitTypes::Zerg_Zergling);
	int numHydras = GetAllUnitCount(_self, BWAPI::UnitTypes::Zerg_Hydralisk);

	goal.push_back(MetaPair(BWAPI::UnitTypes::Zerg_Zergling, zerglings + 6));
	goal.push_back(MetaPair(BWAPI::UnitTypes::Zerg_Hydralisk, numHydras + 3));
}
