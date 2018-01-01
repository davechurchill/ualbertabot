#include "FourBarracksMarine.h"
#include "..\..\UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

void AKBot::FourBarracksMarine::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int numMarines = GetAllUnitCount(BWAPI::UnitTypes::Terran_Marine);
	goal.push_back(MetaPair(BWAPI::UnitTypes::Terran_Marine, numMarines + 8));
}
