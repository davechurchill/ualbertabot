#pragma once

#include "Common.h"
#include "GameState.h"
#include "DFBB_BuildOrderStackSearch.h"
#include "Timer.hpp"

namespace BOSS
{
class DFBB_BuildOrderSmartSearch
{
    RaceID                              _race;

	DFBB_BuildOrderSearchParameters		_params;
	BuildOrderSearchGoal 			_goal;

    ActionSet                           _relevantActions;

	GameState					        _initialState;
	
	int 							    _searchTimeLimit;

	Timer							    _searchTimer;

    DFBB_BuildOrderStackSearch          _stackSearch;

    DFBB_BuildOrderSearchResults        _results;
	
	void doSearch();
	void calculateSearchSettings();
	void setPrerequisiteGoalMax();
	void recurseOverStrictDependencies(const ActionType & action);
    void setRelevantActions();
	void setRepetitions();
	
	UnitCountType calculateSupplyProvidersRequired();
	UnitCountType calculateRefineriesRequired();

    const RaceID getRace() const;
	
public:

	DFBB_BuildOrderSmartSearch(const RaceID race);
	
	void addGoal(const ActionType & a, const UnitCountType count);
	void setGoal(const BuildOrderSearchGoal & goal);
	void setState(const GameState & state);
	void print();
	void setTimeLimit(int n);
	
	void search();

    const DFBB_BuildOrderSearchResults & getResults() const;
	const DFBB_BuildOrderSearchParameters & getParameters();
};

}