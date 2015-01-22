#pragma once

#include "BOSS.h"

namespace BOSS
{

namespace BuildOrderTester
{
    BuildOrderSearchGoal GetRandomGoal(const RaceID race);
    void DoRandomTests(const RaceID race, const size_t numTests);
}
}