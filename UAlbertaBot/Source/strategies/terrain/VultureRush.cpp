#include "VultureRush.h"
#include "UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

void AKBot::VultureRush::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int numVultures = GetAllUnitCount(BWAPI::UnitTypes::Terran_Vulture);

	goal.push_back(MetaPair(BWAPI::UnitTypes::Terran_Vulture, numVultures + 8));

	if (numVultures > 8)
	{
		goal.push_back(MetaPair(BWAPI::TechTypes::Tank_Siege_Mode, 1));
		goal.push_back(MetaPair(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode, 4));
	}
}
