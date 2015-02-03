#pragma once

#include "Common.h"
#include "GameState.h"
#include "CombatSearch.h"
#include "ActionTypeData.h"
#include "Timer.hpp"
#include "ActionType.h"
#include "Tools.h"
#include "DFBB_BuildOrderSmartSearch.h"
#include "Position.hpp"
#include "BuildOrderSearchGoal.h"
#include "BuildOrder.h"
#include "NaiveBuildOrderSearch.h"

namespace BOSS
{
    void init();
    void printData();
}