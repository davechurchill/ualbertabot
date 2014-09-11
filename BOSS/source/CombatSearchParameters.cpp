#include "CombatSearchParameters.h"

using namespace BOSS;

// alternate constructor
CombatSearchParameters::CombatSearchParameters()
: useRepetitions(true)
, useIncreasingRepetitions(false)
, useWorkerCutoff(false)
, workerCutoff(1)
, useAlwaysMakeWorkers(false)
, useSupplyBounding(false)
, supplyBoundingThreshold(1)
, useLandmarkLowerBoundHeuristic(false)
, useResourceLowerBoundHeuristic(false)
, searchTimeLimit(0)
, initialUpperBound(0)
, initialState(Races::None)
, maxActions(Constants::MAX_ACTIONS, -1)
, repetitionValues(Constants::MAX_ACTIONS, 1)
, repetitionThresholds(Constants::MAX_ACTIONS, 0)
{
    
}

void CombatSearchParameters::setSearchTimeLimit(const double timeLimitMS)
{
    searchTimeLimit = timeLimitMS;
}

double CombatSearchParameters::getSearchTimeLimit() const
{
    return searchTimeLimit;
}

void CombatSearchParameters::setRelevantActions(const ActionSet & set)
{
    relevantActions = set;
}

const ActionSet & CombatSearchParameters::getRelevantActions() const
{
    return relevantActions;
}

void CombatSearchParameters::setInitialState(const GameState & s)
{
    initialState = s;
}

const GameState & CombatSearchParameters::getInitialState() const
{
    return initialState;
}

void CombatSearchParameters::setMaxActions(const ActionType & a, int max)
{
    maxActions[a.ID()] = max;
}

void CombatSearchParameters::setRepetitions(const ActionType & a,int repetitions)
{ 
    repetitionValues[a.ID()] = repetitions; 
}

int CombatSearchParameters::getMaxActions(const ActionType & a) const
{ 
    return maxActions[a.ID()]; 
}

int CombatSearchParameters::getRepetitions(const ActionType & a) const
{ 
    return repetitionValues[a.ID()]; 
}

void CombatSearchParameters::setFrameTimeLimit(const FrameCountType limit)
{
    frameTimeLimit = limit;
}

FrameCountType CombatSearchParameters::getFrameTimeLimit() const
{
    return frameTimeLimit;
}

void CombatSearchParameters::print()
{
    printf("\n\nSearch Parameter Information\n\n");

    printf("%s",useRepetitions ?                    "\tUSE      Repetitions\n" : "");
    printf("%s",useIncreasingRepetitions ?          "\tUSE      Increasing Repetitions\n" : "");
    printf("%s",useWorkerCutoff ?                   "\tUSE      Worker Cutoff\n" : "");
    printf("%s",useLandmarkLowerBoundHeuristic ?    "\tUSE      Landmark Lower Bound\n" : "");
    printf("%s",useResourceLowerBoundHeuristic ?    "\tUSE      Resource Lower Bound\n" : "");
    printf("%s",useAlwaysMakeWorkers ?              "\tUSE      Always Make Workers\n" : "");
    printf("%s",useSupplyBounding ?                 "\tUSE      Supply Bounding\n" : "");
    printf("\n");

    //for (int a = 0; a < ACTIONS.size(); ++a)
    //{
    //    if (repetitionValues[a] != 1)
    //    {
    //        printf("\tREP %7d %s\n", repetitionValues[a], ACTIONS[a].getName().c_str());
    //    }
    //}

    //for (int a = 0; a < ACTIONS.size(); ++a)
    //{
    //    if (repetitionThresholds[a] != 0)
    //    {
    //        printf("\tTHR %7d %s\n", repetitionThresholds[a], ACTIONS[a].getName().c_str());
    //    }
    //}

    printf("\n\n");
}