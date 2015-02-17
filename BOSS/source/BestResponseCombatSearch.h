#pragma once

#include "Common.h"
#include "Timer.hpp"
#include "Eval.h"
#include "BuildOrder.h"
#include "CombatSearchParameters.h"
#include "CombatSearchResults.h"

namespace BOSS
{

#define BOSS_COMBATSEARCH_TIMEOUT -1
#define MAX_COMBAT_SEARCH_DEPTH 100
    
class BestResponseCombatSearch
{
	CombatSearchParameters      _params;		    // parameters that will be used in this search
	CombatSearchResults         _results;			// the results of the search so far
	
	FrameCountType              _upperBound; 		// the current upper bound for search
	Timer                       _searchTimer;    
	
    BuildOrder                  _buildOrder;

	void                        doSearch(const GameState & s, size_t depth);

public:
	
	BestResponseCombatSearch(const CombatSearchParameters p = CombatSearchParameters());
	
	void                    search();

    const CombatSearchResults & getResults() const;
	
	static void             generateLegalActions(const GameState & state, ActionSet & legalActions, const CombatSearchParameters & params);
	UnitCountType           getRepetitions(const GameState & s, const ActionType & action);

	double                  eval(const GameState & state) const;

	bool                    isTerminalNode(const GameState & s, int depth);

	void                    updateBuckets(int val, int bucket, GameState & s);
    void                    updateResults(const GameState & state);
	bool                    timeLimitReached();
};

}