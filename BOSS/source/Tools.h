#pragma once

#include "Common.h"
#include "GameState.h"
#include "DFBB_BuildOrderSearchGoal.h"

namespace BOSS
{
namespace Tools
{
    FrameCountType              GetUpperBound(const GameState & state, const DFBB_BuildOrderSearchGoal & goal);
    FrameCountType              GetLowerBound(const GameState & state, const DFBB_BuildOrderSearchGoal & goal);
    FrameCountType              GetBuildOrderCompletionTime(const GameState & state, const std::vector<ActionType> & buildOrder);
    FrameCountType              CalculatePrerequisitesLowerBound(const GameState & state, const PrerequisiteSet & needed, FrameCountType timeSoFar, int depth = 0);
    std::vector<ActionType>     GetNaiveBuildOrder(const GameState & state, const DFBB_BuildOrderSearchGoal & goal);
    std::vector<ActionType>     GetNaiveBuildOrderNew(const GameState & state, const DFBB_BuildOrderSearchGoal & goal, UnitCountType maxWorkers);
    std::vector<ActionType>     InsertActionIntoBuildOrder(const std::vector<ActionType> & buildOrder, const GameState & initialState, const ActionType & action);
    void                        CalculatePrerequisitesRequiredToBuild(const GameState & state, const PrerequisiteSet & wanted, PrerequisiteSet & requiredToBuild);
    bool                        MeetsGoal(const GameState & state, const DFBB_BuildOrderSearchGoal & goal);
    std::vector<ActionType>     GetOptimizedNaiveBuildOrder(const GameState & state, const DFBB_BuildOrderSearchGoal & goal);
    UnitCountType               GetWorkerCount(const std::vector<ActionType> & buildOrder);

    void                        TestBuildOrderUpperBound();
}
}
