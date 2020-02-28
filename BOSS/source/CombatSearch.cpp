#include "CombatSearch.h"

using namespace BOSS;


// function which is called to do the actual search
void CombatSearch::search()
{
    _searchTimer.start();

    // apply the opening build order to the initial state
    GameState initialState(_params.getInitialState());
    _buildOrder = _params.getOpeningBuildOrder();
    _buildOrder.doActions(initialState);

    try
    {
        recurse(initialState, 0);
    
        _results.solved = true;
    }
    catch (int e)
    {
        if (e == BOSS_COMBATSEARCH_TIMEOUT)
        {
            _results.timedOut = true;
        }
    }

    _results.timeElapsed = _searchTimer.getElapsedTimeInMilliSec();
}

// This functio generates the legal actions from a GameState based on the input search parameters
void CombatSearch::generateLegalActions(const GameState & state, ActionSet & legalActions, const CombatSearchParameters & params)
{
    // prune actions we have too many of already
    const ActionSet & allActions = params.getRelevantActions();
    for (ActionID a(0); a<allActions.size(); ++a)
    {
        const ActionType & action = allActions[a];
        bool isLegal = state.isLegal(action);

        if (!isLegal)
        {
            continue;
        }

        // prune the action if we have too many of them already
        if ((params.getMaxActions(action) != -1) && (state.getUnitData().getNumTotal(action) >= params.getMaxActions(action)))
        {
            continue;
        }
        
        legalActions.add(action);
    }

    // if we enabled the always make workers flag, and workers are legal
    const ActionType & worker = ActionTypes::GetWorker(state.getRace());
    if (_params.getAlwaysMakeWorkers() && legalActions.contains(worker))
    {
        bool actionLegalBeforeWorker = false;

        // when can we make a worker
        FrameCountType workerReady = state.whenCanPerform(worker);
        
        // if we can make a worker in the next couple of frames, do it
        if (workerReady <= state.getCurrentFrame() + 2)
        {
            legalActions.clear();
            legalActions.add(worker);
            return;
        }

        // figure out of anything can be made before a worker
        for (size_t a(0); a < legalActions.size(); ++a)
        {
            const ActionType & actionType = legalActions[a];
            const FrameCountType whenCanPerformAction = state.whenCanPerform(actionType);
            if (whenCanPerformAction < workerReady)
            {
                actionLegalBeforeWorker = true;
                break;
            }
        }

        // if something can be made before a worker, then don't consider workers
        if (actionLegalBeforeWorker)
        {
            legalActions.remove(worker);
        }
        // otherwise we can make a worker next so don't consider anything else
        else
        {
            legalActions.clear();
            legalActions.add(worker);
        }
    }
}

const CombatSearchResults & CombatSearch::getResults() const
{
    return _results;
}

bool CombatSearch::timeLimitReached()
{
    return (_params.getSearchTimeLimit() && (_results.nodesExpanded % 100 == 0) && (_searchTimer.getElapsedTimeInMilliSec() > _params.getSearchTimeLimit()));
}

bool CombatSearch::isTerminalNode(const GameState & s, int depth)
{
    if (s.getCurrentFrame() >= _params.getFrameTimeLimit())
    {
        return true;
    }

    return false;
}

void CombatSearch::recurse(const GameState & state, size_t depth)
{
    // This base class function should never be called, leaving the code
    // here as a basis to form child classes

    BOSS_ASSERT(false, "Base CombatSearch doSearch() should never be called");

    //if (timeLimitReached())
    //{
    //    throw BOSS_COMBATSEARCH_TIMEOUT;
    //}

    //updateResults(state);

    //if (isTerminalNode(state, depth))
    //{
    //    return;
    //}

    //ActionSet legalActions;
    //generateLegalActions(state, legalActions, _params);
    //
    //for (UnitCountType a(0); a < legalActions.size(); ++a)
    //{
    //    GameState child(state);
    //    child.doAction(legalActions[a]);
    //    _buildOrder.add(legalActions[a]);
    //    
    //    doSearch(child,depth+1);

    //    _buildOrder.pop_back();
    //}
}

void CombatSearch::updateResults(const GameState & state)
{
    _results.nodesExpanded++;
}

void CombatSearch::printResults()
{
    std::cout << "Printing base class CombatSearch results!\n\n";
}

void CombatSearch::writeResultsFile(const std::string & prefix)
{
    std::cout << "Writing base class CombatSearch results!\n\n";
}