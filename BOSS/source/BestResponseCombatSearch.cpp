#include "BestResponseCombatSearch.h"

using namespace BOSS;

BestResponseCombatSearch::BestResponseCombatSearch(const CombatSearchParameters p)
    : _params(p)
{
   
    BOSS_ASSERT(_params.getInitialState().getRace() != Races::None, "Combat search initial state is invalid");
}

// function which is called to do the actual search
void BestResponseCombatSearch::search()
{
    _searchTimer.start();

    try
    {
        doSearch(_params.getInitialState(), 0);
    
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

void BestRepsonseCombatSearch::generateLegalActions(const GameState & state, ActionSet & legalActions, const CombatSearchParameters & params)
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
}

const CombatSearchResults & CombatSearch::getResults() const
{
    return _results;
}

bool CombatSearch::timeLimitReached()
{
    return (_params.getSearchTimeLimit() && (_results.nodesExpanded % 1000 == 0) && (_searchTimer.getElapsedTimeInMilliSec() > _params.getSearchTimeLimit()));
}

bool CombatSearch::isTerminalNode(const GameState & s, int depth)
{
    if (s.getCurrentFrame() >= _params.getFrameTimeLimit())
    {
        return true;
    }

    return false;
}

double CombatSearch::eval(const GameState & state) const
{
    return BOSS::Eval::ArmyTotalResourceSum(state);
}

void CombatSearch::doSearch(const GameState & state, size_t depth)
{
    if (timeLimitReached())
    {
        throw BOSS_COMBATSEARCH_TIMEOUT;
    }

    updateResults(state);
    _bucket.update(state, _buildOrder);

    if (isTerminalNode(state, depth))
    {
        return;
    }

    if (_bucket.isDominated(state))
    {
        return;
    }

    ActionSet legalActions;
    generateLegalActions(state, legalActions, _params);
    
    for (UnitCountType a(0); a < legalActions.size(); ++a)
    {
        GameState child(state);
        child.doAction(legalActions[a]);
        _buildOrder.add(legalActions[a]);
        _integral.update(state, _buildOrder);
        
        doSearch(child,depth+1);

        _buildOrder.pop_back();
        _integral.pop();
    }
}

void CombatSearch::updateResults(const GameState & state)
{
    _results.nodesExpanded++;

    /*if (_results.nodesExpanded > 0 && _results.nodesExpanded % 1000000 == 0)
    {
        std::cout << ".";
        std::cout.flush();
    }*/
}
