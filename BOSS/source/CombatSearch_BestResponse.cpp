#include "CombatSearch_BestResponse.h"

using namespace BOSS;

CombatSearch_BestResponse::CombatSearch_BestResponse(const CombatSearchParameters p)
    : _bestResponseData(p.getEnemyInitialState(), p.getEnemyBuildOrder())
{
    _params = p;

    BOSS_ASSERT(_params.getInitialState().getRace() != Races::None, "Combat search initial state is invalid");
}

void CombatSearch_BestResponse::recurse(const GameState & state, size_t depth)
{
    if (timeLimitReached())
    {
        throw BOSS_COMBATSEARCH_TIMEOUT;
    }

    _bestResponseData.update(_params.getInitialState(), state, _buildOrder);
    updateResults(state);

    if (isTerminalNode(state, depth))
    {
        return;
    }

    ActionSet legalActions;
    generateLegalActions(state, legalActions, _params);
    
    for (UnitCountType a(0); a < legalActions.size(); ++a)
    {
        size_t ri = legalActions.size() - 1 - a;

        GameState child(state);
        child.doAction(legalActions[ri]);
        _buildOrder.add(legalActions[ri]);
        
        recurse(child,depth+1);

        _buildOrder.pop_back();
    }
}

void CombatSearch_BestResponse::printResults()
{

}

#include "BuildOrderPlot.h"
void CombatSearch_BestResponse::writeResultsFile(const std::string & filename)
{
    BuildOrderPlot plot(_params.getInitialState(), _bestResponseData.getBestBuildOrder());

    plot.writeRectanglePlot(filename + "_BestBuildOrder");
    plot.writeArmyValuePlot(filename + "_BestArmyValue");

    BuildOrderPlot plot2(_params.getEnemyInitialState(), _params.getEnemyBuildOrder());

    plot2.writeRectanglePlot(filename + "_EnemyBuildOrder");
    plot2.writeArmyValuePlot(filename + "_EnemyArmyValue");
}