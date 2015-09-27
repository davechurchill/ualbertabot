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
    void                        InsertActionIntoBuildOrder(BuildOrder & result, const BuildOrder & buildOrder, const GameState & initialState, const ActionType & action);
    void                        CalculatePrerequisitesRequiredToBuild(const GameState & state, const PrerequisiteSet & wanted, PrerequisiteSet & requiredToBuild);
    BuildOrder                  GetOptimizedNaiveBuildOrderOld(const GameState & state, const BuildOrderSearchGoal & goal);
    BuildOrder                  GetNaiveBuildOrderAddWorkersOld(const GameState & state, const BuildOrderSearchGoal & goal, UnitCountType maxWorkers);
}
}
