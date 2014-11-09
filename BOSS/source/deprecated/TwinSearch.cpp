#pragma once

#include "BWAPI.h"
#include <boost/foreach.hpp>
#include "assert.h"
#include <stdio.h>

#include "StarcraftSearch.hpp"
#include "StarcraftSearchConstraint.hpp"
#include "SearchParameters.hpp"
#include "TranspositionTable.hpp"

template <class StarcraftStateType>
class TwinSearch : public StarcraftSearch
{
	SearchParameters<StarcraftStateType> params;		// parameters that will be used in this search
	
	int upperBound; 									// the current upper bound for search
			
	StarcraftStateType winner;							// the winning state so far
	
	SearchResults results;								// the results of the search so far
	
	unsigned long long nodesExpanded;					// how many nodes have been expanded
	unsigned long long numChildren;						// number of children generated
	unsigned long long numGenerations;					// number of parents generated
	
	bool printNewBest;									// when set to true
	
	bool finishedLoadingSaveState;
	
	int buckets;
	std::vector<int>    armyValues;
	std::vector< std::vector<Action> > buildOrders;
	
	
	int ttcuts;
	TranspositionTable<unsigned int, int>   TT;
	
public:
	
	TwinSearch() {}
	
	TwinSearch<StarcraftStateType>(SearchParameters<StarcraftStateType> p) 
		: params(p)
		, nodesExpanded(0)
		, numChildren(0)
		, numGenerations(0)
		, buckets(100)
		, armyValues(buckets, 0)
		, buildOrders(buckets, std::vector<Action>())
		, ttcuts(0)
		, TT(1000000)
	{
		if (params.initialUpperBound == 0)
		{
			upperBound = 0;
		}
		else
		{
			upperBound = params.initialUpperBound;
		}
		
		results.upperBound = upperBound;
	}
	
	// function which is called to do the actual search
	virtual SearchResults monteCarloSearch(int iterations)
	{
		searchTimer.start();
		
		printf("TT size: %d\n", TT.getSize());

		// try to search since we throw exception on timeout
		try {
		
		    for (int i=0; i<iterations; i++)
		    {
		        // search on the initial state
			    DFBBMonteCarlo(params.initialState, 0);
		    }
		
			results.timedOut = false;
		
		// if we catch a timeout exception
		} catch (int e) {
		
			// store that we timed out in the results
			results.timedOut = true;

			// for some reason MSVC++ complains 1about e being unused, so use it
			e = e + 1;
		}
		
		// set the results
		results.nodesExpanded = nodesExpanded;
		results.timeElapsed   = searchTimer.getElapsedTimeInMilliSec();
		
		int skip = upperBound / buckets;
		
		for (int i=0; i<buckets; i++)
		{
		    if (armyValues[i] > 0)
		    {
		        int low = i * skip;
		
		        printf("%6d - %6d : %6d", low, low + skip, armyValues[i]);
		        for (size_t a=0; a<buildOrders[i].size(); ++a)
		        {
		           // printf("%d ", buildOrders[i][a]);
		        }
		        printf("\n");
		    }
		}
		
		return results;
	}
	
	// function which is called to do the actual search
	virtual SearchResults search()
	{	
		searchTimer.start();
		
		// try to search since we throw exception on timeout
		try {
			DFBB(params.initialState, 0);
			
			results.timedOut = false;
		
		// if we catch a timeout exception
		} catch (int e) {
		
			// store that we timed out in the results
			results.timedOut = true;

			// for some reason MSVC++ complains 1about e being unused, so use it
			e = e + 1;
		}
		
		// set the results
		results.nodesExpanded = nodesExpanded;
		results.timeElapsed   = searchTimer.getElapsedTimeInMilliSec();
		
	    int skip = upperBound / buckets;
		
		for (int i=0; i<buckets; i++)
		{
		    if (armyValues[i] > 0)
		    {
		        int low = i * skip;
		
		        printf("%6d - %6d : %5d   ", low, low + skip, armyValues[i]);
		        for (size_t a=0; a<buildOrders[i].size(); ++a)
		        {
		            printf("%d ", buildOrders[i][buildOrders[i].size()-1-a]);
		        }
		        printf("\n");
		    }
		}
		
		printf("Transposition (Cuts = %d) (Collision = %d) (Found = %d) (NotFound = %d)\n", ttcuts, TT.numCollisions(), TT.numFound(), TT.numNotFound());
		
		return results;
	}
	
	int getBucket(int finishTime) const
	{
	    return (int)  (((float)finishTime / upperBound) * buckets);
	}
	
	// recursive function which does all search logic
	void DFBBMonteCarlo(StarcraftStateType & s, int depth)
	{		
	    
		printf("Depth %d\n", depth);
	
		// increase the node expansion count
		nodesExpanded++;
		
		
		// the time at which the last thing in the queue will finish
		int finishTime = s.getLastFinishTime();
		
		if (finishTime >= upperBound)
		{
		    return;
		}
		
		int bucket = getBucket(finishTime);
		int armyValue = s.getArmyValue();
		
		if (armyValue > armyValues[bucket])
		{
		    armyValues[bucket] = armyValue;
		    buildOrders[bucket] = getBuildOrder(s);
		}
		
		// if we are using search timeout and we are over the limit
		if (params.searchTimeLimit && (nodesExpanded % 1000 == 0) && (searchTimer.getElapsedTimeInMilliSec() > params.searchTimeLimit))
		{
			// throw an exception to unroll the recursion
			throw 1;
		}
		
		// get the legal action set
		ActionSet legalActions = s.getLegalActionsMonteCarlo(params.goal); 

		// if we have children, update the counter
		if (!legalActions.isEmpty())
		{
			numGenerations += 1;
			numChildren += legalActions.numActions();
			
			Action nextAction = legalActions.randomAction();
			
			StarcraftStateType child(s);
				
			int readyTime = child.resourcesReady(nextAction); 
			child.doAction(nextAction, readyTime);
			child.setParent(&s);
			DFBBMonteCarlo(child, depth+1);
		}
		else
		{
		    printf("No legal actions %d\n", depth);
		}
	}
	
	
	// recursive function which does all search logic
	void DFBB(StarcraftStateType & s, int depth)
	{		
		// increase the node expansion count
		nodesExpanded++;
		
		//graphVizOutput(s, false);
		
		// the time at which the last thing in the queue will finish
		int finishTime = s.getLastFinishTime();
		
		if (finishTime >= upperBound)
		{
		    return;
		}
		
		int lookupVal = TT.lookup(s.hashAllUnits(1), s.hashAllUnits(2));
		if (lookupVal != -1 && lookupVal < finishTime)
		{
		    ttcuts++;
		    return;
		}

		TT.save(s.hashAllUnits(1), s.hashAllUnits(2), finishTime);
		
		int bucket = getBucket(finishTime);
		int armyValue = s.getArmyValue();
		
		if (armyValue > armyValues[bucket])
		{
		    armyValues[bucket] = armyValue;
		    buildOrders[bucket] = getBuildOrder(s);
		}
		
		// if we are using search timeout and we are over the limit
		if (params.searchTimeLimit && (nodesExpanded % 1000 == 0) && (searchTimer.getElapsedTimeInMilliSec() > params.searchTimeLimit))
		{
			// throw an exception to unroll the recursion
			throw 1;
		}
		
		// get the legal action set
		ActionSet legalActions = s.getLegalActionsMonteCarlo(params.goal); 

		// if we have children, update the counter
		if (!legalActions.isEmpty())
		{
			numGenerations += 1;
			numChildren += legalActions.numActions();
		}
	
		// while there are still legal actions to perform
		while (!legalActions.isEmpty()) 
		{				
			// get the next action
			Action nextAction = legalActions.popAction();

			bool stillLegal = true;
			StarcraftStateType child(s);
			
			// set the repetitions if we are using repetitions, otherwise set to 1
			int repeat = params.useRepetitions ? params.getRepetitions(nextAction) : 1;

			// for each repetition of this action
			for (int r = 0; r < repeat; ++r)
			{
				// if the action is still legal
				if (child.isLegalMonteCarlo(nextAction, params.goal))
				{						
					int readyTime = child.resourcesReady(nextAction); 
					child.doAction(nextAction, readyTime);
				}
				// if it's not legal, break the chain
				else
				{
					stillLegal = false;
					break;
				}
			}
			
			//if (stillLegal)
			//{
			    child.setParent(&s);
			    child.setActionPerformedK((UnitCountType)repeat);
			    DFBB(child, depth+1);
			//}
		}	
	}
};

