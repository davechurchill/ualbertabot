#pragma once

#include "Common.h"
#include "GameState.h"
#include "BuildOrderSearchGoal.h"
#include "BuildOrder.h"

namespace BOSS
{
namespace Tools
{
    FrameCountType              GetUpperBound(const GameState & state, const BuildOrderSearchGoal & goal);
    FrameCountType              GetLowerBound(const GameState & state, const BuildOrderSearchGoal & goal);
    FrameCountType              CalculatePrerequisitesLowerBound(const GameState & state, const PrerequisiteSet & needed, FrameCountType timeSoFar, int depth = 0);
    BuildOrder                  GetNaiveBuildOrderAddWorkers(const GameState & state, const BuildOrderSearchGoal & goal, UnitCountType maxWorkers);
    void                        InsertActionIntoBuildOrder(BuildOrder & result, const BuildOrder & buildOrder, const GameState & initialState, const ActionType & action);
    void                        CalculatePrerequisitesRequiredToBuild(const GameState & state, const PrerequisiteSet & wanted, PrerequisiteSet & requiredToBuild);
    BuildOrder                  GetOptimizedNaiveBuildOrder(const GameState & state, const BuildOrderSearchGoal & goal);
}
}
