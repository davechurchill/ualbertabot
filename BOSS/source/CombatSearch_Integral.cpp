#include "CombatSearch_Integral.h"

using namespace BOSS;

CombatSearch_Integral::CombatSearch_Integral(const CombatSearchParameters p)
{
    _params = p;

    BOSS_ASSERT(_params.getInitialState().getRace() != Races::None, "Combat search initial state is invalid");
}

void CombatSearch_Integral::doSearch(const GameState & state, size_t depth)
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
        _integral.update(state, _buildOrder);
        
        doSearch(child,depth+1);

        _buildOrder.pop_back();
        _integral.pop();
    }
}

void CombatSearch_Integral::printResults()
{
    _integral.print();
}