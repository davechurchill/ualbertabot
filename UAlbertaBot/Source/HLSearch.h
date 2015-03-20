#pragma once
#include "Common.h"
#include "Squad.h"
#include "HLState.h"
#include "HLStatistics.h"

namespace UAlbertaBot
{
	const static int MIN_SCORE = std::numeric_limits<int>::min() + 1, MAX_SCORE = std::numeric_limits<int>::max() - 1;
	class HLSearch
	{
		//std::list<std::pair<int, int> >		_strategicPV;//frame,strategy
		//std::list<std::pair<int, Squad> >	_tacticalPV;//frame,squad
		//todo:switch these to priority_queues?
	
		HLSearch();
		~HLSearch();

		HLStatistics _stats;

		HLMove _bestMove;	//saved by the search
		int alphaBeta(const HLState& state, int depth, int frameLimit, int turn, const HLMove &firstSideMove, int alpha, int beta);
		void uct(const HLState &state, int playouts);
	public:
		static HLSearch &Instance();

		void search(double timeLimit);

		//std::vector<Squad> getSquads(const std::set<BWAPI::UnitInterface*> & combatUnits);//get Squads and orders
		//std::set<BWAPI::UnitInterface*> getScouts();
		HLMove getBestMove();
	};

}