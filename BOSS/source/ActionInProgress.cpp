#include "ActionInProgress.h"

using namespace BOSS;

ActionsInProgress::ActionsInProgress() 
    : _numProgress(Constants::MAX_ACTIONS, 0)
{
	
}
		
UnitCountType ActionsInProgress::numInProgress(const ActionType & a) const
{
	return _numProgress[a.ID()];	
}
	
void ActionsInProgress::addAction(const ActionType & action, FrameCountType time)
{
	// deprecated, code below should be faster
    //inProgress.push_back(ActionInProgress(a, (unsigned short)time));
    //inProgress.sort();
	
    // inProgress should always be sorted, so add the action in its place
    _inProgress.addSorted(ActionInProgress(action, time));

	// increase the specific count of a
	_numProgress[action.ID()]++;
}
	
void ActionsInProgress::popNextAction()	
{
	BOSS_ASSERT(_inProgress.size() > 0, "Can't pop from empty set");
	
	// there is one less of the last unit in progress
	_numProgress[_inProgress[_inProgress.size()-1]._action.ID()]--;
	
	// the number of things in progress goes down
    _inProgress.pop_back();
}
	
bool ActionsInProgress::isEmpty() const
{
	return _inProgress.size() == 0;
}
	
FrameCountType ActionsInProgress::nextActionFinishTime() const
{
	BOSS_ASSERT(_inProgress.size() > 0, "Set is empty");
		
	return _inProgress[_inProgress.size()-1]._time;
}
	
const UnitCountType ActionsInProgress::size() const
{
	return _inProgress.size();
}
	
FrameCountType ActionsInProgress::getLastFinishTime() const
{
	if (_inProgress.size() == 0)
	{
		return 0;
	}
		
	return _inProgress[0]._time;
}
	
FrameCountType ActionsInProgress::nextActionFinishTime(const ActionType & a) const
{
	BOSS_ASSERT(numInProgress(a) > 0, "Tried to get next finish time from empty set");
		
	for (size_t i=_inProgress.size()-1; i >=0; --i)
	{
		if (_inProgress[i]._action == a)
		{
			return _inProgress[i]._time;
		}
	}
		
    BOSS_ASSERT(false, "Next Action Finish Time didn't find anything");

	return -1;
}

FrameCountType ActionsInProgress::nextBuildingFinishTime() const
{
    BOSS_ASSERT(_inProgress.size() > 0, "There are no buildings in progress");

	for (size_t i(0); i<_inProgress.size(); ++i)
	{
        size_t ri = _inProgress.size()-1-i;
		if (_inProgress[ri]._action.isBuilding() && !_inProgress[ri]._action.isAddon() && !_inProgress[ri]._action.isMorphed())
		{
			return _inProgress[ri]._time;
		}
	}

    for (size_t i(0); i<_inProgress.size(); ++i)
	{
        size_t ri = _inProgress.size()-1-i;
		std::cout << _inProgress[ri]._action.getName() << std::endl;
	}
		
    BOSS_ASSERT(false, "There were no buildings in progress");

	return -1;
}
	
const ActionType & ActionsInProgress::getAction(const UnitCountType i) const
{
    BOSS_ASSERT(i < _inProgress.size(), "index out of bounds");

	return _inProgress[i]._action;
}
	
FrameCountType ActionsInProgress::getTime(const UnitCountType i) const
{
    BOSS_ASSERT(i < _inProgress.size(), "index out of bounds");

	return _inProgress[i]._time;
}
	
const ActionType & ActionsInProgress::nextAction() const
{
	BOSS_ASSERT(_inProgress.size() > 0, "Tried to get nextAction() from empty set");
	
	return _inProgress[_inProgress.size()-1]._action;
}
	
void ActionsInProgress::printActionsInProgress()
{
	for (int i(0); i<Constants::MAX_ACTIONS; ++i)
	{
		if (_numProgress[i] > 0)
		{
			printf("Progress: %d %d %d\n", i, (int)_numProgress[i], (int)_inProgress[i]._time);
		}
	}
}

//bool ActionsInProgress::willBuildingFinsishBeforeRefinery()
//{
//    int firstBuildingIndex = 100000;
//    int lastRefineryIndex = 0;
//
//    for (size_t i(0); i < _inProgress.size(); ++i)
//    {
//        if (_inProgress.)
//    }
//}
	
FrameCountType ActionsInProgress::whenActionsFinished(const PrerequisiteSet & actions) const
{
	BOSS_ASSERT(!actions.isEmpty(), "Action set is empty!");
	
	// the maximum of the (minimums for each action)
	int totalMax = 0;
	
	// if there are actions still left
	for (UnitCountType uc(0); uc < actions.size(); ++uc)
	{	
		// pop an action off the set
		const ActionType & a = actions.getActionType(uc);
			
		// define a new minimum
		int actionMin = std::numeric_limits<int>::max();
		
		// for each unit in our progress vector
		for (size_t i(0); i<_inProgress.size(); ++i) 
		{
			// if the action matches
			if (_inProgress[i]._action == a) 
			{
				// check to see if we have a new minimum
				actionMin = (_inProgress[i]._time < actionMin) ? _inProgress[i]._time : actionMin;
			}
		}
			
		// if we found a new minimum
		if (actionMin < std::numeric_limits<int>::max())
		{
			// check to see if we have a new maximum
			totalMax = (actionMin > totalMax) ? actionMin : totalMax;
		}
	}
		
	// return the new maximum
	return totalMax;
}
	