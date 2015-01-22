#include "DFBB_BuildOrderSmartSearch.h"

using namespace BOSS;

DFBB_BuildOrderSmartSearch::DFBB_BuildOrderSmartSearch(const RaceID race) 
    : _race(race)
    , _params(race)
    , _goal(race)
    , _stackSearch(race)
    , _searchTimeLimit(30)
{
}

void DFBB_BuildOrderSmartSearch::doSearch()
{
    BOSS_ASSERT(_initialState.getRace() != Races::None, "Must set initial state before performing search");

    // if we are resuming a search
    if (_stackSearch.getResults().timedOut)
    {
        _stackSearch.setTimeLimit(_searchTimeLimit);
        _stackSearch.search();
    }
    else
    {
        calculateSearchSettings();
        _params.goal = _goal;
        _params.initialState = _initialState;
        _params.useRepetitions 				= true;
        _params.useIncreasingRepetitions 	= true;
        _params.useAlwaysMakeWorkers 		= true;
        _params.useSupplyBounding 			= true;
        _params.supplyBoundingThreshold     = 1.5;
        _params.relevantActions             = _relevantActions;
        _params.searchTimeLimit             = _searchTimeLimit;

        _params.print();

        //BWAPI::Broodwar->printf("Constructing new search object time limit is %lf", _params.searchTimeLimit);
        _stackSearch = DFBB_BuildOrderStackSearch(_params);
        _stackSearch.search();
    }

    _results = _stackSearch.getResults();

    if (_results.solved && !_results.solutionFound)
    {
        std::cout << "No solution found better than naive, using naive build order" << std::endl;
        _results.buildOrder = Tools::GetOptimizedNaiveBuildOrder(_params.initialState, _params.goal);
    }
}

void DFBB_BuildOrderSmartSearch::calculateSearchSettings()
{
    // set the max number of resource depots to what we have since no expanding is allowed
    const ActionType & resourceDepot    = ActionTypes::GetResourceDepot(getRace());
    const ActionType & refinery         = ActionTypes::GetRefinery(getRace());
    const ActionType & worker           = ActionTypes::GetWorker(getRace());
    const ActionType & supplyProvider   = ActionTypes::GetSupplyProvider(getRace());

    _goal.setGoalMax(resourceDepot, _initialState.getUnitData().getNumTotal(resourceDepot));

    // set the number of refineries
    _goal.setGoalMax(refinery, std::min((UnitCountType)3, calculateRefineriesRequired()));

    // set the maximum number of workers to an initial ridiculously high upper bound
    _goal.setGoalMax(worker, std::min(_initialState.getUnitData().getNumTotal(worker) + 20, 100));

    // set the number of supply providers required
    _goal.setGoalMax(supplyProvider, calculateSupplyProvidersRequired());

    // set the maximums for all goal prerequisites
    setPrerequisiteGoalMax();

    // set relevant actions
    setRelevantActions();

    // set the repetitions
    setRepetitions();
}

// calculates maximum number of refineries we'll need
UnitCountType DFBB_BuildOrderSmartSearch::calculateRefineriesRequired()
{
    const ActionType & refinery      = ActionTypes::GetRefinery(getRace());
    const ActionType & resourceDepot = ActionTypes::GetResourceDepot(getRace());

    if (_goal.getGoal(refinery))
    {
        return _goal.getGoal(refinery);
    }

    // loop to check if we need gas
    bool gasRequired = false;
    for (size_t a(0); a < ActionTypes::GetAllActionTypes(getRace()).size(); ++a)
    {
        const ActionType & actionType = ActionTypes::GetActionType(getRace(), a);

        if (_goal.getGoal(actionType) > 0 && actionType.gasPrice() > 0)
        {
            gasRequired = true;
            break;
        }
    }

    return gasRequired ? _initialState.getUnitData().getNumTotal(resourceDepot) : 0;
}

// handles all goalMax calculations for prerequisites of goal actions
void DFBB_BuildOrderSmartSearch::setPrerequisiteGoalMax()
{
    if (getRace() == Races::Protoss || getRace() == Races::Terran)
    {
        // for each unit in the goal vector
        for (size_t a(0); a < ActionTypes::GetAllActionTypes(getRace()).size(); ++a)
        {
            const ActionType & actionType = ActionTypes::GetActionType(getRace(), a);

            // if we want one of these
            if (_goal.getGoal(actionType) > 0)
            {
                // set goalMax for each strict dependency equal to 1
                recurseOverStrictDependencies(actionType);
            }
        }

        // vector which stores the number of goal units which are built by [index]
        std::vector<UnitCountType> numGoalUnitsBuiltBy(ActionTypes::GetAllActionTypes(getRace()).size(), 0);

        for (size_t a(0); a < numGoalUnitsBuiltBy.size(); ++a)
        {
            const ActionType & actionType = ActionTypes::GetActionType(getRace(), a);

            if (_goal.getGoal(actionType) > 0)
            {
                // add this to the sum
                numGoalUnitsBuiltBy[actionType.whatBuildsAction()] += _goal.getGoal(actionType);

                // if it's in the goal, make sure it's in the max
                _goal.setGoalMax(actionType, std::max(_goal.getGoal(actionType), _goal.getGoalMax(actionType)));
            }
        }

        UnitCountType productionBuildingLimit = 4;

        for (size_t a(0); a < numGoalUnitsBuiltBy.size(); ++a)
        {
            const ActionType & actionType = ActionTypes::GetActionType(getRace(), a);

            // if it's not a resource depot
            if (!actionType.isResourceDepot() && actionType.isBuilding())
            {
                // if this building produces units
                if (numGoalUnitsBuiltBy[actionType.ID()] > 0)
                {
                    // set the goal max to how many units
                    _goal.setGoalMax(actionType, std::min(productionBuildingLimit, numGoalUnitsBuiltBy[actionType.ID()]));
                }
            }
        }
    }
    else if (getRace() == Races::Zerg)
    {
        _goal.setGoalMax(ActionTypes::GetActionType("Zerg_Spawning_Pool"), 1);
        _goal.setGoalMax(ActionTypes::GetActionType("Zerg_Extractor"), 1);
        _goal.setGoalMax(ActionTypes::GetActionType("Zerg_Lair"), 1);
        _goal.setGoalMax(ActionTypes::GetActionType("Zerg_Spire"), 1);
        _goal.setGoalMax(ActionTypes::GetActionType("Zerg_Hydralisk_Den"), 1);
    }
}

// recursively checks the tech tree of Action and sets each to have goalMax of 1
void DFBB_BuildOrderSmartSearch::recurseOverStrictDependencies(const ActionType & actionType)
{
    if (actionType.isResourceDepot() || actionType.isWorker() || actionType.isSupplyProvider() || actionType.isRefinery())
    {
        return;
    }

    PrerequisiteSet recursivePrerequisites = actionType.getRecursivePrerequisites();

    for (size_t a(0); a < recursivePrerequisites.size(); ++a)
    {
        const ActionType & actionType = recursivePrerequisites.getActionType(a);

        if (actionType.isResourceDepot() ||actionType.isWorker() || actionType.isSupplyProvider() || actionType.isRefinery())
        {
            continue;
        }

        _goal.setGoalMax(actionType, std::max((UnitCountType)1, _goal.getGoalMax(actionType)));
    }
}

void DFBB_BuildOrderSmartSearch::setRelevantActions()
{
    _relevantActions.clear();
    for (size_t a(0); a < ActionTypes::GetAllActionTypes(getRace()).size(); ++a)
    {
        const ActionType & actionType = ActionTypes::GetActionType(getRace(), a);

        if (_goal.getGoalMax(actionType))
        {
            _relevantActions.add(actionType);
        }
    }
}


UnitCountType DFBB_BuildOrderSmartSearch::calculateSupplyProvidersRequired()
{
    const ActionType & resourceDepot    = ActionTypes::GetResourceDepot(getRace());
    const ActionType & worker           = ActionTypes::GetWorker(getRace());
    const ActionType & supplyProvider   = ActionTypes::GetSupplyProvider(getRace());

    // calculate the upper bound on supply for this goal
    int supplyNeeded = _goal.getGoalMax(worker) * worker.supplyRequired();

    // for each prerequisite of things in the goal which aren't production facilities set one of
    for (size_t a(0); a < ActionTypes::GetAllActionTypes(getRace()).size(); ++a)
    {
        const ActionType & actionType = ActionTypes::GetActionType(getRace(), a);

        // add the supply required for this number of goal units and all units currently made
        supplyNeeded += std::max(_goal.getGoal(actionType), _initialState.getUnitData().getNumTotal(actionType)) * actionType.supplyRequired();
    }

    // set the upper bound on supply based on these values
    UnitCountType supplyFromResourceDepots = _initialState.getUnitData().getNumTotal(resourceDepot) * resourceDepot.supplyProvided();

    // take this away from the supply needed
    supplyNeeded -= supplyFromResourceDepots;

    // return the number of supply providers required
    return supplyNeeded > 0 ? (UnitCountType)ceil((double)supplyNeeded / (double)supplyProvider.supplyProvided()) : 0;
}

void DFBB_BuildOrderSmartSearch::setRepetitions()
{
    //const ActionType & resourceDepot    = ActionTypes::GetResourceDepot(getRace());
    //const ActionType & refinery         = ActionTypes::GetRefinery(getRace());
    //const ActionType & worker           = ActionTypes::GetWorker(getRace());
    //const ActionType & supplyProvider   = ActionTypes::GetSupplyProvider(getRace());

    //_params.setRepetitions(supplyProvider, 1);
    //_params.setRepetitionThreshold(supplyProvider, 3);

    // for each action
    for (size_t a(0); a < ActionTypes::GetAllActionTypes(getRace()).size(); ++a)
    {
        const ActionType & actionType = ActionTypes::GetActionType(getRace(), a);

        // if if want 4 or more of something that isn't supply providing
        if (!actionType.isSupplyProvider() && _goal.getGoal(actionType) >= 5)
        {
            // set the repetitions to half of the value
            _params.setRepetitions(actionType, std::min((UnitCountType)4, (UnitCountType)(_goal.getGoal(actionType) / 2)));
            _params.setRepetitions(actionType.whatBuildsActionType(), 2);
            _params.setRepetitionThreshold(actionType.whatBuildsActionType(), 1);
        }
    }
}

const RaceID DFBB_BuildOrderSmartSearch::getRace() const
{
    return _race;
}

void DFBB_BuildOrderSmartSearch::addGoal(const ActionType & a, const UnitCountType count)
{
    _goal.setGoal(a,count);
}

void DFBB_BuildOrderSmartSearch::setGoal(const BuildOrderSearchGoal & g)
{
    _goal = g;    
}

void DFBB_BuildOrderSmartSearch::setState(const GameState & state)
{
    _initialState = state;
}


void DFBB_BuildOrderSmartSearch::setTimeLimit(int n)
{
    _searchTimeLimit = n;
}

void DFBB_BuildOrderSmartSearch::search()
{
    doSearch();
}

const DFBB_BuildOrderSearchResults & DFBB_BuildOrderSmartSearch::getResults() const
{
    return _results;
}

const DFBB_BuildOrderSearchParameters & DFBB_BuildOrderSmartSearch::getParameters()
{
    calculateSearchSettings();

    _params.goal = _goal;
    _params.initialState = _initialState;
    _params.useRepetitions 				= true;
    _params.useIncreasingRepetitions 	= true;
    _params.useAlwaysMakeWorkers 		= true;
    _params.useSupplyBounding 			= true;

    return _params;
}

void DFBB_BuildOrderSmartSearch::print()
{
    //initialState.printData();
    _goal.printGoal();
    printf("\n\n");
}
