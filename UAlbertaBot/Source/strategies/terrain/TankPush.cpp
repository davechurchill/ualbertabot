#include "TankPush.h"
#include "..\..\UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

void AKBot::TankPush::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int numGoliath = GetAllUnitCount(BWAPI::UnitTypes::Terran_Goliath);

	goal.push_back(MetaPair(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode, 6));
	goal.push_back(MetaPair(BWAPI::UnitTypes::Terran_Goliath, numGoliath + 6));
	goal.push_back(MetaPair(BWAPI::TechTypes::Tank_Siege_Mode, 1));
}
