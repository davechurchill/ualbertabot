#pragma once

#include "Common.h"
#include "PrerequisiteSet.h"
#include "GameState.h"
#include "ActionType.h"
#include "Timer.hpp"

namespace BOSS
{
class CombatSearchResults
{

public:

    bool                solved;			// whether ot not a solution was found
    bool                timedOut;		// did the search time-out?

    int                 solutionLength;	// the length of the solution
    int                 upperBound;		// upper bound of first node
    int                 lowerBound;		// lower bound of first node

    unsigned long long  nodesExpanded;	// number of nodes expanded in the search

    double              timeElapsed;	// time elapsed in milliseconds
    double              avgBranch;		// avg branching factor

    Timer               searchTimer;         

    GameState           winner;

    std::vector<ActionType> buildOrder;		// the build order

    double              highestEval;

    ResourceCountType   minerals;
    ResourceCountType   gas;

    FrameCountType      frameCompleted;

    CombatSearchResults();
    CombatSearchResults(bool s,int len,unsigned long long n,double t,std::vector<ActionType> solution);

    void printResults(bool pbo = true);
    void printBuildOrder();
};
}