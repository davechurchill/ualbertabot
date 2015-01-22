#pragma once

#include "Common.h"
#include "ActionType.h"
#include "GameState.h"
#include "BuildOrder.h"

namespace BOSS
{
class DFBB_BuildOrderSearchResults
{

public:
   
	BuildOrder                  buildOrder;		// the build order

	bool 				        solved;			// whether ot not a solution was found
    bool    			        timedOut;		// did the search time-out?
    bool                        solutionFound;  // did we find any solution
	
	int					        upperBound;		// upper bound of first node
	
	unsigned long long 	        nodesExpanded;	// number of nodes expanded in the search
	
	double 				        timeElapsed;	// time elapsed in milliseconds

    GameState                   finalState;
	
	DFBB_BuildOrderSearchResults();
	DFBB_BuildOrderSearchResults(bool s, int len, unsigned long long n, double t, std::vector<ActionType> solution);
		
	void printResults(bool pbo = true) const;
	void printBuildOrder() const;
};
}