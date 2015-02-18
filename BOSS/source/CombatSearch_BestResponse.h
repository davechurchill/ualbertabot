#pragma once

#include "Common.h"
#include "Timer.hpp"
#include "Eval.h"
#include "BuildOrder.h"
#include "CombatSearch.h"
#include "CombatSearchParameters.h"
#include "CombatSearchResults.h"

namespace BOSS
{

class CombatSearch_BestResponse : public CombatSearch
{
	virtual void                doSearch(const GameState & s, size_t depth);

    BuildOrder                  _bestBuildOrder;

    double                      compareBuildOrders(const GameState & selfState, const BuildOrder & self, const GameState & enemyState, const BuildOrder & enemy);

public:
	
	CombatSearch_BestResponse(const CombatSearchParameters p = CombatSearchParameters());
	
    virtual void printResults();
};

}