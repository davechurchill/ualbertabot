#include "CombatSearch_Bucket.h"

using namespace BOSS;

CombatSearch_Bucket::CombatSearch_Bucket(const CombatSearchParameters p)
    : _bucket(p.getFrameTimeLimit(), 200)
{
    _params = p;
   
    BOSS_ASSERT(_params.getInitialState().getRace() != Races::None, "Combat search initial state is invalid");
}
void CombatSearch_Bucket::doSearch(const GameState & state, size_t depth)
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
        //return;
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

void CombatSearch_Bucket::printResults()
{
    _bucket.print();
}

#include "BuildOrderPlot.h"
void CombatSearch_Bucket::writeResultsFile(const std::string & filename)
{
    BuildOrderPlot::WriteGnuPlot(filename + "_BucketResults", _bucket.getBucketResultsString(), " with steps");

    // write the final build order data
    BuildOrderPlot plot(_params.getInitialState(), _bucket.getBucket(_bucket.numBuckets()-1).buildOrder);
    plot.writeArmyValuePlot(filename + "_FinalBucketArmyPlot");
    plot.writeRectanglePlot(filename + "_FinalBucketBuildOrder");
}