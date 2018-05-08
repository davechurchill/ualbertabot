#include "TerranMiddleGame.h"
#include "..\..\UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

AKBot::TerranMiddleGame::TerranMiddleGame(BWAPI::Player self)
	: _self(self)
{
}

void AKBot::TerranMiddleGame::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	if (_self->supplyUsed(_self->getRace()) == 200) {
		return;
	}

	int numMarines = GetAllUnitCount(_self, BWAPI::UnitTypes::Terran_Marine);
	int numGoliath = GetAllUnitCount(_self, BWAPI::UnitTypes::Terran_Goliath);
	int numTanks = GetAllUnitCount(_self, BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode);
	int numSiegeTanks = GetAllUnitCount(_self, BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode);

	goal.push_back(MetaPair(BWAPI::UnitTypes::Terran_Marine, numMarines + 8));
	goal.push_back(MetaPair(BWAPI::UnitTypes::Terran_Goliath, numGoliath + 2));
	goal.push_back(MetaPair(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode, numTanks + 2));
}
