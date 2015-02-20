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
        const UnitCountType index = legalActions.size()-1-a;

        GameState child(state);
        child.doAction(legalActions[index]);
        _buildOrder.add(legalActions[index]);
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

#include "BuildOrderPlot.h"
void CombatSearch_Integral::writeResultsFile(const std::string & filename)
{
    BuildOrderPlot plot(_params.getInitialState(), _integral.getBestBuildOrder());

    plot.writeResourcePlot(filename + "_Resources");
    plot.writeRectanglePlot(filename + "_BuildOrder");
    plot.writeArmyValuePlot(filename + "_ArmyValue");
}