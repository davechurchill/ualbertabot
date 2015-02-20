#include "CombatSearch_BestResponseData.h"

using namespace BOSS;

CombatSearch_BestResponseData::CombatSearch_BestResponseData(const GameState & enemyState, const BuildOrder & enemyBuildOrder)
    : _enemyInitialState(enemyState)
    , _enemyBuildOrder(enemyBuildOrder)
    , _bestEval(std::numeric_limits<double>::max())
{
    // compute enemy army values

    calculateArmyValues(_enemyInitialState, _enemyBuildOrder, _enemyArmyValues);
}

void CombatSearch_BestResponseData::calculateArmyValues(const GameState & initialState, const BuildOrder & buildOrder, std::vector< std::pair<double, double> > & values)
{
    values.clear();
    GameState state(initialState);
    for (size_t i(0); i < buildOrder.size(); ++i)
    {
        state.doAction(buildOrder[i]);
        values.push_back(std::pair<double,double>(state.getCurrentFrame(), Eval::ArmyTotalResourceSum(state)));
    }
}

#include "BuildOrderPlot.h"
void CombatSearch_BestResponseData::update(const GameState & initialState, const GameState & currentState, const BuildOrder & buildOrder)
{
    double eval = compareBuildOrder(initialState, buildOrder);

    if (eval < _bestEval)
    {
        _bestEval = eval;
        _bestBuildOrder = buildOrder;
        _bestState = currentState;

        std::cout << eval/Constants::RESOURCE_SCALE << "   " << _bestBuildOrder.getNameString(2) << std::endl;
    }
}

double CombatSearch_BestResponseData::compareBuildOrder(const GameState & initialState, const BuildOrder & buildOrder)
{
    calculateArmyValues(initialState, buildOrder, _selfArmyValues);

    size_t selfIndex = 0;
    size_t enemyIndex = 0;
    double maxDiff = std::numeric_limits<double>::lowest();
    double sumDiff = 0;
    int n = 0;

    for (size_t ei(0); ei < _enemyArmyValues.size(); ++ei)
    {
        double enemyTime = _enemyArmyValues[ei].first;
        double enemyVal = _enemyArmyValues[ei].second;    
    
        size_t selfIndex = 0;

        // find the corresponding self army value for this time
        for (size_t si(0); si < _selfArmyValues.size(); ++si)
        {
            if (enemyTime < _selfArmyValues[si].first)
            {
                break;
            }

            selfIndex = si;
        }
    
        double selfVal = _selfArmyValues[selfIndex].second;
        double diff = enemyVal - selfVal;
        maxDiff = std::max(maxDiff, diff);
    }

    return maxDiff;
}

size_t CombatSearch_BestResponseData::getStateIndex(const GameState & state)
{
    FrameCountType frame = state.getCurrentFrame();

    if (frame > _enemyStates.back().getCurrentFrame())
    {
        return _enemyStates.size() - 1;
    }

    for (size_t i(0); i < _enemyStates.size(); ++i)
    {
        if (frame < _enemyStates[i].getCurrentFrame())
        {
            return i;
        }
    }

    BOSS_ASSERT(false, "Should have found an index");
    return 0;
}

const BuildOrder & CombatSearch_BestResponseData::getBestBuildOrder() const
{
    return _bestBuildOrder;
}