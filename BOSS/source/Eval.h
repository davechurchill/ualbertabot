#pragma once

#include "Common.h"
#include "GameState.h"
#include "BuildOrder.h"

namespace BOSS
{
namespace Eval
{
    
    double ArmyCompletedResourceSum(const GameState & state);
    double ArmyTotalResourceSum(const GameState & state);

    bool BuildOrderBetter(const BuildOrder & buildOrder, const BuildOrder & compareTo);

    bool StateDominates(const GameState & state, const GameState & other);
}
}
