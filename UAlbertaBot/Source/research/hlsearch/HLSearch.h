#pragma once
#include "Common.h"
#include "Squad.h"
#include "HLState.h"
#include "HLStatistics.h"
#include "HLTranspositionTable.h"

namespace UAlbertaBot
{
	const static int MIN_SCORE = std::numeric_limits<int>::min() + 1, MAX_SCORE = std::numeric_limits<int>::max() - 1;
	class HLSearch
	{
		//std::list<std::pair<int, int> >		_strategicPV;//frame,strategy
		//std::list<std::pair<int, Squad> >	_tacticalPV;//frame,squad
		//todo:switch these to priority_queues?


		HLStatistics _stats;

		HLMove _bestMove;	//saved by the search
		int alphaBeta(const HLState& state, int depth, int height, int frameLimit, int turn, const HLMove &firstSideMove, int alpha, int beta);
		//void uct(const HLState &state, int playouts);
		//const int _defaultStrategy = 0;
		HLTranspositionTable _tt;
		HLCacheTable _cache;
		bool _timeUp;
		int _timeLimitMs;
		int _maxFrame,_minFrame;
	public:
		HLSearch(const UAlbertaBot::HLSearch &) = delete;
		HLSearch();
		~HLSearch();
		long long search(double timeLimit, int frameLimit, int maxHeight);

		//std::vector<Squad> getSquads(const std::set<BWAPI::UnitInterface*> & combatUnits);//get Squads and orders
		//std::set<BWAPI::UnitInterface*> getScouts();
		HLMove getBestMove();
	};

}