#include "HLManager.h"

using namespace UAlbertaBot;

HLManager::HLManager()
{
	BWAPI::Broodwar->printf("High Level Manager Instantiated");
}


HLManager::~HLManager()
{
}

HLManager &	HLManager::Instance()
{
	static HLManager instance;
	return instance;
}

void HLManager::update(
	std::set<BWAPI::UnitInterface*> combatUnits,
	std::set<BWAPI::UnitInterface*> scoutUnits,
	std::set<BWAPI::UnitInterface*> workerUnits)
{
	//scoutManager.update(HLSearch::Instance().getScouts());
	static int frame = 0;
	if (frame++==1){
		//BWAPI::Broodwar->printf("Race: %d %s", BWAPI::Broodwar->enemy()->getRace().getID(), BWAPI::Broodwar->enemy()->getRace().c_str());
		//BWAPI::Broodwar->printf("UnitRace: %s", (*BWAPI::Broodwar->enemy()->getUnits().begin())->getType().getRace().c_str());
		HLSearch::Instance().search(1000); 
		StrategyManager::Instance().setCurrentStrategy(HLSearch::Instance().getStrategy());
	}
	
}