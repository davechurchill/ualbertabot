#pragma once

#include "Common.h"
#include "ActionType.h"
#include "DFBB_BuildOrderSearchResults.h"
#include "DFBB_BuildOrderSearchParameters.h"
#include "Timer.hpp"
#include "Tools.h"
#include "BuildOrder.h"

#define DFBB_TIMEOUT_EXCEPTION 1

namespace BOSS
{

class StackData
{
public:

    size_t              currentChildIndex;
    GameState           state;
    ActionSet           legalActions;
    ActionType          currentActionType;
    UnitCountType       repetitionValue;
    UnitCountType       completedRepetitions;
    
    StackData()
        : currentChildIndex(0)
        , repetitionValue(1)
        , completedRepetitions(0)
    {
    
    }
};

class DFBB_BuildOrderStackSearch
{
	DFBB_BuildOrderSearchParameters     _params;                      //parameters that will be used in this search
	DFBB_BuildOrderSearchResults        _results;                     //the results of the search so far
					
    Timer                               _searchTimer;
    BuildOrder                          _buildOrder;

    std::vector<StackData>              _stack;
    size_t                              _depth;

    bool                                _firstSearch;

    bool                                _wasInterrupted;
    
    void                                updateResults(const GameState & state);
    bool                                isTimeOut();
    void                                calculateRecursivePrerequisites(const ActionType & action, ActionSet & all);
    void                                generateLegalActions(const GameState & state, ActionSet & legalActions);
	std::vector<ActionType>             getBuildOrder(GameState & state);
    UnitCountType                       getRepetitions(const GameState & state, const ActionType & a);
    ActionSet                           calculateRelevantActions();

public:
	
	DFBB_BuildOrderStackSearch(const DFBB_BuildOrderSearchParameters & p);
	
    void setTimeLimit(double ms);
	void search();
    const DFBB_BuildOrderSearchResults & getResults() const;
	
	void DFBB();
	
	
};
}