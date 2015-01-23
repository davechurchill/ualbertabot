#pragma once

#include "BOSS.h"

namespace BOSS
{

namespace BuildOrderTester
{
    GameState GetStartState(const RaceID race, int randomActions);
    BuildOrderSearchGoal GetRandomGoal(const RaceID race);
    void DoRandomTests(const RaceID race, const size_t numTests);

    void TestRandomBuilds(const RaceID race, const size_t numTests);
}
}