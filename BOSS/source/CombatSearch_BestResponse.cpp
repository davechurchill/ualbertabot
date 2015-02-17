#include "CombatSearch_BestResponse.h"

using namespace BOSS;

CombatSearch_BestResponse::CombatSearch_BestResponse(const CombatSearchParameters p)
{
    _params = p;

    BOSS_ASSERT(_params.getInitialState().getRace() != Races::None, "Combat search initial state is invalid");
}

void CombatSearch_BestResponse::doSearch(const GameState & state, size_t depth)
{
    if (timeLimitReached())
    {
        throw BOSS_COMBATSEARCH_TIMEOUT;
    }

    updateResults(state);

    if (isTerminalNode(state, depth))
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
        
        doSearch(child,depth+1);

        _buildOrder.pop_back();
    }
}

void CombatSearch_BestResponse::printResults()
{

}