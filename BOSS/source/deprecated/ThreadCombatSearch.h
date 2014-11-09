#pragma once

#include "Common.h"
#include "Timer.hpp"
#include "Eval.h"
#include "CombatSearchParameters.h"
#include "CombatSearchResults.h"
#include "CombatSearchBucket.h"
#include "CombatSearchIntegral.h"
#include "CombatSearch.h"

namespace BOSS
{

#define BOSS_COMBATSEARCH_TIMEOUT -1
#define MAX_COMBAT_SEARCH_DEPTH 100


class ThreadCombatSearch
{
	CombatSearchParameters      _params;				// parameters that will be used in this search
	CombatSearchResults         _results;			// the results of the search so far
	
	Timer                       _searchTimer;    

	
public:
	
	ThreadCombatSearch(const CombatSearchParameters p = CombatSearchParameters());
	
	void                        search();

    const CombatSearchResults & getResults() const;

    void                        launchThread(size_t threadNum);
};

}