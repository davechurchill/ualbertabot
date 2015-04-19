#include "HLCombatCommander.h"

using namespace UAlbertaBot;

HLCombatCommander::HLCombatCommander()
{
}


HLCombatCommander::~HLCombatCommander()
{
}

void HLCombatCommander::update(std::set<BWAPI::UnitInterface*> combatUnits)
{
	//todo: don't do this every frame
	/*squadData.clearSquadData();
	for (auto& s : HLSearch::Instance().getSquads(combatUnits)){
		squadData.addSquad(s);
	}*/

	combatCommander.update(combatUnits);
}