#pragma once

#include "Common.h"
#include "Timer.hpp"
#include "Eval.h"
#include "BuildOrder.h"
#include "CombatSearch.h"
#include "CombatSearchParameters.h"
#include "CombatSearchResults.h"
#include "CombatSearch_BucketData.h"

namespace BOSS
{

class CombatSearch_Bucket : public CombatSearch
{
    CombatSearch_BucketData     _bucket;

	virtual void                doSearch(const GameState & s, size_t depth);

public:
	
	CombatSearch_Bucket(const CombatSearchParameters p = CombatSearchParameters());

    virtual void printResults();
    virtual void writeResultsFile(const std::string & filename);
};

}