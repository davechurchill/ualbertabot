#pragma once

#include "Common.h"
#include "GameState.h"

namespace BOSS
{
namespace Eval
{
    
    double ArmyCompletedResourceSum(const GameState & state);
    double ArmyTotalResourceSum(const GameState & state);

    bool BuildOrderBetter(const std::vector<ActionType> & buildOrder, const std::vector<ActionType> & compareTo);

    bool StateDominates(const GameState & state, const GameState & other);
}
}
