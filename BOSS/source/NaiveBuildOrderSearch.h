#pragma once

#include "Common.h"
#include "BuildOrderSearchGoal.h"
#include "GameState.h"
#include "BuildOrder.h"
#include "Tools.h"

namespace BOSS
{

class NaiveBuildOrderSearch
{
    GameState                   _state;
    BuildOrderSearchGoal        _goal;
    BuildOrder                  _buildOrder;

    bool                        _naiveSolved;

public:

    NaiveBuildOrderSearch(const GameState & state, const BuildOrderSearchGoal & goal);

    const BuildOrder & solve();
};

}