#pragma once

#include "Common.h"
#include "GameState.h"
#include "Eval.h"
#include "BuildOrder.h"

namespace BOSS
{
    
class CombatSearch_BestResponseData
{
    GameState               _enemyInitialState;
    BuildOrder              _enemyBuildOrder;

    std::vector<GameState>  _enemyStates;
    std::vector< std::pair<double, double> >     _enemyArmyValues;
    std::vector< std::pair<double, double> >     _selfArmyValues;

    double                  _bestEval;
    BuildOrder              _bestBuildOrder;
    GameState               _bestState;

    double compareBuildOrder(const GameState & state, const BuildOrder & buildOrder);
    size_t getStateIndex(const GameState & state);

    void calculateArmyValues(const GameState & state, const BuildOrder & buildOrder, std::vector< std::pair<double, double> > & values);

public:

    CombatSearch_BestResponseData(const GameState & enemyState, const BuildOrder & enemyBuildOrder);

    void update(const GameState & initialState, const GameState & currentState, const BuildOrder & buildOrder);

    const BuildOrder & getBestBuildOrder() const;

};

}
