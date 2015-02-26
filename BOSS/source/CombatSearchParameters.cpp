#include "CombatSearchParameters.h"

using namespace BOSS;

// alternate constructor
CombatSearchParameters::CombatSearchParameters()
    : _useRepetitions                (true)
    , _useIncreasingRepetitions      (false)
    , _useWorkerCutoff               (false)
    , _workerCutoff                  (1)
    , _useAlwaysMakeWorkers          (false)
    , _useSupplyBounding             (false)
    , _supplyBoundingThreshold       (1)
    , _useLandmarkLowerBoundHeuristic(false)
    , _useResourceLowerBoundHeuristic(false)
    , _searchTimeLimit               (0)
    , _initialUpperBound             (0)
    , _initialState                  (Races::None)
    , _maxActions                    (Constants::MAX_ACTIONS, -1)
    , _repetitionValues              (Constants::MAX_ACTIONS, 1)
    , _repetitionThresholds          (Constants::MAX_ACTIONS, 0)
    , _printNewBest                  (false)
{
    
}

void CombatSearchParameters::setSearchTimeLimit(const double timeLimitMS)
{
    _searchTimeLimit = timeLimitMS;
}

double CombatSearchParameters::getSearchTimeLimit() const
{
    return _searchTimeLimit;
}

void CombatSearchParameters::setRelevantActions(const ActionSet & set)
{
    _relevantActions = set;
}

const ActionSet & CombatSearchParameters::getRelevantActions() const
{
    return _relevantActions;
}

void CombatSearchParameters::setInitialState(const GameState & s)
{
    _initialState = s;
}

const GameState & CombatSearchParameters::getInitialState() const
{
    return _initialState;
}

void CombatSearchParameters::setEnemyInitialState(const GameState & s)
{
    _enemyInitialState = s;
}

const GameState & CombatSearchParameters::getEnemyInitialState() const
{
    return _enemyInitialState;
}

void CombatSearchParameters::setMaxActions(const ActionType & a, int max)
{
    _maxActions[a.ID()] = max;
}

void CombatSearchParameters::setOpeningBuildOrder(const BuildOrder & buildOrder)
{
    _openingBuildOrder = buildOrder;
}

const BuildOrder & CombatSearchParameters::getOpeningBuildOrder() const
{
    return _openingBuildOrder;
}

void CombatSearchParameters::setEnemyBuildOrder(const BuildOrder & buildOrder)
{
    _enemyBuildOrder = buildOrder;
}

const BuildOrder & CombatSearchParameters::getEnemyBuildOrder() const
{
    return _enemyBuildOrder;
}

void CombatSearchParameters::setRepetitions(const ActionType & a,int repetitions)
{ 
    _repetitionValues[a.ID()] = repetitions; 
}

int CombatSearchParameters::getMaxActions(const ActionType & a) const
{ 
    return _maxActions[a.ID()]; 
}

int CombatSearchParameters::getRepetitions(const ActionType & a) const
{ 
    return _repetitionValues[a.ID()]; 
}

void CombatSearchParameters::setFrameTimeLimit(const FrameCountType limit)
{
    _frameTimeLimit = limit;
}

void CombatSearchParameters::setAlwaysMakeWorkers(const bool flag)
{
    _useAlwaysMakeWorkers = flag;
}

const bool CombatSearchParameters::getAlwaysMakeWorkers() const
{
    return _useAlwaysMakeWorkers;
}   

FrameCountType CombatSearchParameters::getFrameTimeLimit() const
{
    return _frameTimeLimit;
}



void CombatSearchParameters::print()
{
    printf("\n\nSearch Parameter Information\n\n");

    printf("%s", _useRepetitions ?                    "\tUSE      Repetitions\n" : "");
    printf("%s", _useIncreasingRepetitions ?          "\tUSE      Increasing Repetitions\n" : "");
    printf("%s", _useWorkerCutoff ?                   "\tUSE      Worker Cutoff\n" : "");
    printf("%s", _useLandmarkLowerBoundHeuristic ?    "\tUSE      Landmark Lower Bound\n" : "");
    printf("%s", _useResourceLowerBoundHeuristic ?    "\tUSE      Resource Lower Bound\n" : "");
    printf("%s", _useAlwaysMakeWorkers ?              "\tUSE      Always Make Workers\n" : "");
    printf("%s", _useSupplyBounding ?                 "\tUSE      Supply Bounding\n" : "");
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