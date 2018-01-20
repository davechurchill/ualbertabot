#include "DarkTemplarRush.h"
#include "..\..\UnitUtil.h"

using UAlbertaBot::MetaPairVector;
using UAlbertaBot::MetaPair;
using UAlbertaBot::UnitUtil::GetAllUnitCount;

AKBot::DarkTemplarRush::DarkTemplarRush(BWAPI::Player self)
	: _self(self)
{
}

void AKBot::DarkTemplarRush::getBuildOrderGoal(MetaPairVector& goal, int currentFrame) const
{
	int numDragoons = GetAllUnitCount(_self, BWAPI::UnitTypes::Protoss_Dragoon);
	int numNexusAll = GetAllUnitCount(_self, BWAPI::UnitTypes::Protoss_Nexus);
	int numDarkTeplar = GetAllUnitCount(_self, BWAPI::UnitTypes::Protoss_Dark_Templar);

	goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Dark_Templar, numDarkTeplar + 2));

	// if we have a 2nd nexus then get some goons out
	if (numNexusAll >= 2)
	{
		goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Dragoon, numDragoons + 4));
	}
}
