#pragma once

#include "Common.h"
#include <string.h>
#include <queue>
#include <algorithm>
#include <limits>

#include "PrerequisiteSet.h"
#include <math.h>
#include "Array.hpp"
#include "ActionType.h"

namespace BOSS
{

class ActionInProgress
{

public:

	ActionType 		_action;
	FrameCountType 	_time;
	
	ActionInProgress() 
        : _time(0) 
    {  
    }
	
    ActionInProgress(const ActionType & action, FrameCountType t) 
        : _action(action)
        , _time(t) 
    {
    }
	

	// we want to sort these in descending order in ActionsInProgress
	const bool operator < (const ActionInProgress & rhs) const 
    { 
        return _time > rhs._time; 
    }
};

class ActionsInProgress
{
	Vec<ActionInProgress, Constants::MAX_PROGRESS>	    _inProgress;
    Vec<UnitCountType, Constants::MAX_ACTIONS>          _numProgress;	// how many of each unit are in progress
	
public:

	ActionsInProgress();
	
	UnitCountType operator [] (const ActionType & action) const;
	UnitCountType numInProgress(const ActionType & action) const;
	
	void addAction(const ActionType & a, int time);
	void popNextAction();
	bool isEmpty() const;
	const UnitCountType size() const;

    //bool willBuildingFinsishBeforeRefinery();
	
	FrameCountType getLastFinishTime() const;
	FrameCountType nextActionFinishTime(const ActionType & a) const;
	FrameCountType getTime(const UnitCountType i) const;
	FrameCountType nextActionFinishTime() const;
    FrameCountType nextBuildingFinishTime() const;
	FrameCountType whenActionsFinished(const PrerequisiteSet & actions) const;
	
	const ActionType & getAction(const UnitCountType index) const;
	const ActionType & nextAction() const;
	
	void printActionsInProgress();
};
}

