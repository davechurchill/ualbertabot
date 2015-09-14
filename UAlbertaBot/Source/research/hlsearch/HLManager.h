#pragma once
#include "Common.h"
#include "Squad.h"
#include "HLSearch.h"
#include "ScoutManager.h"
namespace UAlbertaBot
{
	class HLManager
	{
		//ScoutManager scoutManager;
		HLManager();
		~HLManager();
		HLSearch _search;
		
	public:
		static	HLManager &	Instance();

		void update();
		//void update(
		//	std::set<BWAPI::UnitInterface*> combatUnits, 
		//	std::set<BWAPI::UnitInterface*> scoutUnits, 
		//	std::set<BWAPI::UnitInterface*> workerUnits);
	};

}