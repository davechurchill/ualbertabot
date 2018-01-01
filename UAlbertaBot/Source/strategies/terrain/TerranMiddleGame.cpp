#include "TerranMiddleGame.h"
#include "..\..\UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

void AKBot::TerranMiddleGame::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int numMarines = GetAllUnitCount(BWAPI::UnitTypes::Terran_Marine);
	int numGoliath = GetAllUnitCount(BWAPI::UnitTypes::Terran_Goliath);
	int numTanks = GetAllUnitCount(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode);
	int numSiegeTanks = GetAllUnitCount(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode);

	goal.push_back(MetaPair(BWAPI::UnitTypes::Terran_Marine, numMarines + 8));
	goal.push_back(MetaPair(BWAPI::UnitTypes::Terran_Goliath, numGoliath + 2));
	goal.push_back(MetaPair(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode, numTanks + 2));
}
