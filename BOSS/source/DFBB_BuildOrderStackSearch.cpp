#include "DFBB_BuildOrderStackSearch.h"

using namespace BOSS;

DFBB_BuildOrderStackSearch::DFBB_BuildOrderStackSearch(const DFBB_BuildOrderSearchParameters & p)
    : _params(p)
    , _depth(0)
    , _firstSearch(true)
    , _wasInterrupted(false)
    , _stack(100, StackData())
{
    
}

void DFBB_BuildOrderStackSearch::setTimeLimit(double ms)
{
    _params.searchTimeLimit = ms;
}

// function which is called to do the actual search
void DFBB_BuildOrderStackSearch::search()
{
    _searchTimer.start();

    if (!_results.solved)
    {
        if (_firstSearch)
        {
            _results.upperBound = _params.initialUpperBound ? _params.initialUpperBound : Tools::GetUpperBound(_params.initialState, _params.goal);
            _stack[0].state = _params.initialState;
            _firstSearch = false;
            //BWAPI::Broodwar->printf("Upper bound is %d", _results.upperBound);
            std::cout << "Upper bound is: " << _results.upperBound << std::endl;
        }

        try 
        {
            // search on the initial state
            DFBB();

            _results.timedOut = false;
        }
        catch (int e) 
        {
            if (e == DFBB_TIMEOUT_EXCEPTION)
            {
                //BWAPI::Broodwar->printf("I timed out!");
                _results.timedOut = true;
            }
        }
        
        double ms = _searchTimer.getElapsedTimeInMilliSec();
        _results.solved = !_results.timedOut;
        _results.timeElapsed = ms;
    }
}

const DFBB_BuildOrderSearchResults & DFBB_BuildOrderStackSearch::getResults() const
{
    return _results;
}

void DFBB_BuildOrderStackSearch::generateLegalActions(const GameState & state, ActionSet & legalActions)
{
    legalActions.clear();
    BuildOrderSearchGoal & goal = _params.goal;
    const ActionType & worker = ActionTypes::GetWorker(state.getRace());
    
    // add all legal relevant actions that are in the goal
    for (size_t a(0); a < _params.relevantActions.size(); ++a)
    {
        const ActionType & actionType = _params.relevantActions[a];

        if (state.isLegal(actionType))
        {
            // if there's none of this action in the goal it's not legal
            if (!goal.getGoal(actionType) && !goal.getGoalMax(actionType))
            {
                continue;
            }

            // if we alread have more than the goal it's not legal
            if (goal.getGoal(actionType) && (state.getUnitData().getNumTotal(actionType) >= goal.getGoal(actionType)))
            {
                continue;
            }

            // if we already have more than the goal max it's not legal
            if (goal.getGoalMax(actionType) && (state.getUnitData().getNumTotal(actionType) >= goal.getGoalMax(actionType)))
            {
                continue;
            }
            
            legalActions.add(_params.relevantActions[a]);
        }
    }

    // don't make anything until we have 8 workers
    if (state.getUnitData().getNumTotal(worker) < 8)
    {
        legalActions.clear();
        legalActions.add(worker);
        return;
    }

    // if we enabled the supply bounding flag
    if (_params.useSupplyBounding)
    {
        UnitCountType supplySurplus = state.getUnitData().getMaxSupply() + state.getUnitData().getSupplyInProgress() - state.getUnitData().getCurrentSupply();
        UnitCountType threshold = (UnitCountType)(ActionTypes::GetSupplyProvider(state.getRace()).supplyProvided() * _params.supplyBoundingThreshold);

        if (supplySurplus >= threshold)
        {
            legalActions.remove(ActionTypes::GetSupplyProvider(state.getRace()));
        }
    }
    
    // if we enabled the always make workers flag, and workers are legal
    if (_params.useAlwaysMakeWorkers && legalActions.contains(worker))
    {
        bool actionLegalBeforeWorker = false;
        ActionSet legalEqualWorker;
        FrameCountType workerReady = state.whenCanPerform(worker);

        for (size_t a(0); a < legalActions.size(); ++a)
        {
            const ActionType & actionType = legalActions[a];
            const FrameCountType whenCanPerformAction = state.whenCanPerform(actionType);
            if (whenCanPerformAction < workerReady)
            {
                actionLegalBeforeWorker = true;
                break;
            }

            if ((whenCanPerformAction == workerReady) && (actionType.mineralPrice() == worker.mineralPrice()))
            {
                legalEqualWorker.add(actionType);
            }
        }

        if (actionLegalBeforeWorker)
        {
            legalActions.remove(worker);
        }
        else
        {
            legalActions = legalEqualWorker;
        }
    }
}

UnitCountType DFBB_BuildOrderStackSearch::getRepetitions(const GameState & state, const ActionType & a)
{
    // set the repetitions if we are using repetitions, otherwise set to 1
    int repeat = _params.useRepetitions ? _params.getRepetitions(a) : 1;

    // if we are using increasing repetitions
    if (_params.useIncreasingRepetitions)
    {
        // if we don't have the threshold amount of units, use a repetition value of 1
        repeat = state.getUnitData().getNumTotal(a) >= _params.getRepetitionThreshold(a) ? repeat : 1;
    }

    // make sure we don't repeat to more than we need for this unit type
    if (_params.goal.getGoal(a))
    {
        repeat = std::min(repeat, _params.goal.getGoal(a) - state.getUnitData().getNumTotal(a));
    }
    else if (_params.goal.getGoalMax(a))
    {
        repeat = std::min(repeat, _params.goal.getGoalMax(a) - state.getUnitData().getNumTotal(a));
    }
    
    return repeat;
}

bool DFBB_BuildOrderStackSearch::isTimeOut()
{
    return (_params.searchTimeLimit && (_results.nodesExpanded % 200 == 0) && (_searchTimer.getElapsedTimeInMilliSec() > _params.searchTimeLimit));
}

void DFBB_BuildOrderStackSearch::updateResults(const GameState & state)
{
    FrameCountType finishTime = state.getLastActionFinishTime();

    // new best solution
    if (finishTime < _results.upperBound)
    {
        _results.timeElapsed = _searchTimer.getElapsedTimeInMilliSec();
        _results.upperBound = finishTime;
        _results.solutionFound = true;
        _results.finalState = state;
        _results.buildOrder = _buildOrder;

        _results.printResults(true);
    }
}

#define ACTION_TYPE     _stack[_depth].currentActionType
#define STATE           _stack[_depth].state
#define CHILD_STATE     _stack[_depth+1].state
#define CHILD_NUM       _stack[_depth].currentChildIndex
#define LEGAL_ACTINS    _stack[_depth].legalActions
#define REPETITIONS     _stack[_depth].repetitionValue
#define COMPLETED_REPS  _stack[_depth].completedRepetitions

#define DFBB_CALL_RETURN  if (_depth == 0) { return; } else { --_depth; goto SEARCH_RETURN; }
#define DFBB_CALL_RECURSE { ++_depth; goto SEARCH_BEGIN; }

// recursive function which does all search logic
void DFBB_BuildOrderStackSearch::DFBB()
{
    FrameCountType actionFinishTime = 0;
    FrameCountType heuristicTime = 0;
    FrameCountType maxHeuristic = 0;

SEARCH_BEGIN:

    _results.nodesExpanded++;

    if (isTimeOut())
    {
        throw DFBB_TIMEOUT_EXCEPTION;
    }

    generateLegalActions(STATE, LEGAL_ACTINS);
    for (CHILD_NUM = 0; CHILD_NUM < LEGAL_ACTINS.size(); ++CHILD_NUM)
    {
        ACTION_TYPE = LEGAL_ACTINS[CHILD_NUM];

        actionFinishTime = STATE.whenCanPerform(ACTION_TYPE) + ACTION_TYPE.buildTime();
        heuristicTime    = STATE.getCurrentFrame() + Tools::GetLowerBound(STATE, _params.goal);
        maxHeuristic     = (actionFinishTime > heuristicTime) ? actionFinishTime : heuristicTime;

        if (maxHeuristic > _results.upperBound)
        {
            continue;
        }

        REPETITIONS = getRepetitions(STATE, ACTION_TYPE);
                
        // do the action as many times as legal to to 'repeat'
        CHILD_STATE = STATE;
        COMPLETED_REPS = 0;
        for (; COMPLETED_REPS < REPETITIONS; ++COMPLETED_REPS)
        {
            if (CHILD_STATE.isLegal(ACTION_TYPE))
            {
                _buildOrder.add(ACTION_TYPE);
                CHILD_STATE.doAction(ACTION_TYPE);
            }
            else
            {
                break;
            }
        }

        if (_params.goal.isAchievedBy(CHILD_STATE))
        {
            updateResults(CHILD_STATE);
        }
        else
        {
            DFBB_CALL_RECURSE;
        }

SEARCH_RETURN:

        for (size_t r(0); r < COMPLETED_REPS; ++r)
        {
            _buildOrder.pop_back();
        }
    }

    DFBB_CALL_RETURN;
}