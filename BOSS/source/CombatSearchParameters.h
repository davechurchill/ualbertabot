#pragma once

#include "Common.h"
#include "GameState.h"
#include "BuildOrder.h"

namespace BOSS
{

class CombatSearchParameters
{
	void init();

    Vec<int, Constants::MAX_ACTIONS> _maxActions;

	//      Flag which determines whether or not doubling macro actions will be used in search.
	//      Macro actions (see paper) trade suboptimality for decreasing search depth. For large
	//          plans repetitions are necessary. For example, to make probes only build in twos
	//          set useRepetitions = true and repetitionValues[probeAction] = 2
	//
	//      true:  macro actions are used, stored in repetitionValues array
    //      false: macro actions not used, all actions will be carried out once
	bool 	_useRepetitions;
	Vec<UnitCountType, Constants::MAX_ACTIONS>	_repetitionValues;
	
	//      Flag which determines whether increasing repetitions will be used in search
	//      Increasing repetitions means the reptition value will be 1 until we have at least
    //      repetitionThresholds[a] count of action a. For example, setting:
	//          repetitionThresholds[pylonAction] = 1, repetitionValues[pylonAction] = 2
	//          means that the first pylon will build on its own but all further pylons will
	//          be built 2 at a time.
	//
	//      true:  increasing repetitions are used
	//      false: increasing repetitions not used
	bool	_useIncreasingRepetitions;
	Vec<UnitCountType, Constants::MAX_ACTIONS> _repetitionThresholds;

	//		Flag which determines whether or not we use worker cutoff pruning in search.
	//		Worker cutoff pruning stops workers from being constructed after a certain number
	//			of frames have passed in the search. Intuitively we build the majority of workers
	//			at the beginning of a build, so this can enforce it to make search faster. If
	//          true, workers are no longer legal if currentFrame > workerCutoff * uperBound
	//          in our search algorithm. If workerCutoff is 1, workers will not be pruned.
	//
	//      true:  worker cutoff is used
	//      false: worker cutoff not used
	bool	_useWorkerCutoff;					
	double 	_workerCutoff;
	
	//      Flag which determines whether or not we always make workers during search
	//      This abstraction changes the search so that it always makes a worker if it is able to. It
	//          accomplished this by modifying the current legal actions to exclude anything that
	//          can't be started before the next worker. This ends up producing longer makespans but
	//          the economy it produces is much better. Optimal makespan plans often produce very
	//          few workers and the long term economy suffers.
	//
	//      true:  always make workers is used
	//      false: always make workers is not used
	bool	_useAlwaysMakeWorkers;
	
	//      Flag which determines whether or not we use supply bounding in our search
	//      Supply bounding makes supply producing buildings illegal if we are currently ahead
	//          on supply by a certain amount. If we currently have more than
	//          supplyBoundingThreshold extra supply buildings worth of supply, we no longer
	//          build them. This is an abstraction used to make search faster which may
	//          produce suboptimal plans.
	//
	//      true:  supply bounding is used
	//      false: supply bounding is not used
	bool	_useSupplyBounding;
	int		_supplyBoundingThreshold;
	
	
	//      Flag which determines whether or not we use various heuristics in our search.
	//
	//      true:  the heuristic is used
	//      false: the heuristic is not used
	bool	_useLandmarkLowerBoundHeuristic;
	bool	_useResourceLowerBoundHeuristic;
	
	//      Search time limit measured in milliseconds
	//      If searchTimeLimit is set to a value greater than zero, the search will effectively
	//          time out and the best solution so far will be used in the results. This is
	//          accomplished by throwing an exception if the time limit is hit. Time is checked
	//          once every 1000 nodes expanded, as checking the time is slow.
	double	_searchTimeLimit;
	
	//      Initial upper bound for the DFBB search
	//      If this value is set to zero, DFBB search will automatically determine an
	//          appropriate upper bound using an upper bound heuristic. If it is non-zero,
	//          it will use the value as an initial bound.
	int		_initialUpperBound;
			
	//      Initial GameState used for the search. See GameState.h for details
	GameState				_initialState;
    BuildOrder              _openingBuildOrder;

    GameState               _enemyInitialState;
    BuildOrder              _enemyBuildOrder;

    ActionSet               _relevantActions;
    FrameCountType          _frameTimeLimit;
    bool                    _printNewBest;



public:

	// alternate constructor
	CombatSearchParameters();
	
	void 	            setRepetitions(const ActionType & a, int repetitions);
	int 	            getRepetitions(const ActionType & a) const;

    void                setMaxActions(const ActionType & a, int max);
    int 	            getMaxActions(const ActionType & a) const;

    void                setRelevantActions(const ActionSet & set);
    const ActionSet &   getRelevantActions() const;

    void                setInitialState(const GameState & s);
    const GameState &   getInitialState() const;

    void                setEnemyInitialState(const GameState & s);
    const GameState &   getEnemyInitialState() const;

    void                setOpeningBuildOrder(const BuildOrder & buildOrder);
    const BuildOrder &  getOpeningBuildOrder() const;

    void                setEnemyBuildOrder(const BuildOrder & buildOrder);
    const BuildOrder &  getEnemyBuildOrder() const;

    void                setSearchTimeLimit(const double timeLimitMS);
    double              getSearchTimeLimit() const;

    void                setFrameTimeLimit(const FrameCountType limit);
    FrameCountType      getFrameTimeLimit() const;

    void                setAlwaysMakeWorkers(const bool flag);
    const bool          getAlwaysMakeWorkers() const;
	
	void print();
};

}