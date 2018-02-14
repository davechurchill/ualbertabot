#include "FourBarracksMarine.h"
#include "..\..\UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

AKBot::FourBarracksMarine::FourBarracksMarine(BWAPI::Player self)
	: _self(self)
{
}

void AKBot::FourBarracksMarine::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int numMarines = GetAllUnitCount(_self, BWAPI::UnitTypes::Terran_Marine);
	goal.push_back(MetaPair(BWAPI::UnitTypes::Terran_Marine, numMarines + 8));
}
