#pragma once

#include "Common.h"
#include "GameState.h"
#include "CombatSearch.h"
#include "CombatSearch_Integral.h"
#include "CombatSearch_Bucket.h"
#include "CombatSearch_BestResponse.h"
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