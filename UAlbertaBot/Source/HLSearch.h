#pragma once
#include "Common.h"
#include "Squad.h"

namespace UAlbertaBot
{
	class HLSearch
	{
		std::list<std::pair<int, int> >		strategicPV;//frame,strategy
		std::list<std::pair<int, Squad> >	tacticalPV;//frame,squad
		//todo:switch these to priority_queues?

		HLSearch();
		~HLSearch();
	public:
		static HLSearch &Instance();

		void search(double timeLimit);

		std::vector<Squad> getSquads(std::set<BWAPI::UnitInterface*> & combatUnits);//get Squads and orders
		std::set<BWAPI::UnitInterface*> getScouts();
		int getStrategy();
	};

}