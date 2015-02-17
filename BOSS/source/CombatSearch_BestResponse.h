#pragma once

#include "Common.h"
#include "Timer.hpp"
#include "Eval.h"
#include "BuildOrder.h"
#include "CombatSearch.h"
#include "CombatSearchParameters.h"
#include "CombatSearchResults.h"
#include "CombatSearch_IntegralData.h"

namespace BOSS
{

#define BOSS_COMBATSEARCH_TIMEOUT -1
#define MAX_COMBAT_SEARCH_DEPTH 100


class CombatSearch_BestResponse : public CombatSearch
{
	virtual void                doSearch(const GameState & s, size_t depth);

public:
	
	CombatSearch_BestResponse(const CombatSearchParameters p = CombatSearchParameters());
	
    virtual void printResults();
};

}