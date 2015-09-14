#include "HLCombatCommander.h"

using namespace UAlbertaBot;

HLCombatCommander::HLCombatCommander(){
}


HLCombatCommander::~HLCombatCommander()
{
}

void HLCombatCommander::update(std::set<BWAPI::UnitInterface*> combatUnits)
{
	//if (squadUpdateFrame())
	//{
	//	// clear all squad data
	//	squadData.clearSquadData();

	//	// give back combat workers to worker manager
	//	WorkerManager::Instance().finishedWithCombatWorkers();

	//	for (auto& s : HLSearch::Instance().getSquads(combatUnits)){
	//		squadData.addSquad(s);
	//	}
	//}

	//squadData.update();


	combatCommander.update(combatUnits);
}


bool HLCombatCommander::squadUpdateFrame() const
{
	return BWAPI::Broodwar->getFrameCount() % 24 == 0;
}