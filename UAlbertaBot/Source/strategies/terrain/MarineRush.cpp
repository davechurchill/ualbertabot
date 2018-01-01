#include "MarineRush.h"
#include "..\..\UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

void AKBot::MarineRush::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int numMarines = GetAllUnitCount(BWAPI::UnitTypes::Terran_Marine);
	goal.push_back(MetaPair(BWAPI::UnitTypes::Terran_Marine, numMarines + 8));

	if (numMarines > 5)
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Terran_Engineering_Bay, 1));
	}
}
