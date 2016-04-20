#pragma once

#include "Common.h"
#include "Timer.hpp"
#include "Eval.h"
#include "BuildOrder.h"
#include "CombatSearch.h"
#include "CombatSearchParameters.h"
#include "CombatSearchResults.h"
#include "CombatSearch_BestResponseData.h"

namespace BOSS
{

class CombatSearch_BestResponse : public CombatSearch
{
	virtual void                    recurse(const GameState & s, size_t depth);

    CombatSearch_BestResponseData   _bestResponseData;

    BuildOrder                      _bestBuildOrder;

    double                          compareBuildOrders(const GameState & selfState, const BuildOrder & selfBuildOrder, const GameState & enemyState, const BuildOrder & enemyBuildOrder);

public:
	
	CombatSearch_BestResponse(const CombatSearchParameters p = CombatSearchParameters());
	
    virtual void printResults();

    virtual void writeResultsFile(const std::string & filename);
};

}