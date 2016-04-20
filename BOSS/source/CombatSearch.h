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


class CombatSearch
{
protected:

    CombatSearchParameters      _params;            // parameters that will be used in this search
    CombatSearchResults         _results;			// the results of the search so far

    FrameCountType              _upperBound; 		// the current upper bound for search
    Timer                       _searchTimer;

    BuildOrder                  _buildOrder;

    virtual void                recurse(const GameState & s,size_t depth);
    virtual void                generateLegalActions(const GameState & state,ActionSet & legalActions,const CombatSearchParameters & params);

    //virtual double              eval(const GameState & state) const;
    virtual bool                isTerminalNode(const GameState & s,int depth);

    virtual void                updateResults(const GameState & state);
    virtual bool                timeLimitReached();

public:

    virtual void                search();
    virtual void                printResults();
    virtual void                writeResultsFile(const std::string & prefix);

    virtual const CombatSearchResults & getResults() const;
};

}