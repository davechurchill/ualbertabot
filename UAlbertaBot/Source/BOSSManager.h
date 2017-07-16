#pragma once

#include "Common.h"
#include "WorkerManager.h"
#include "../../BOSS/source/BOSS.h"
#include "StrategyManager.h"
#include <memory>
#include "OpponentView.h"

namespace UAlbertaBot
{
    
typedef std::shared_ptr<BOSS::DFBB_BuildOrderSmartSearch> SearchPtr;

class BOSSManager
{
    int                                     _previousSearchStartFrame;
    int                                     _savedSearchStartFrame;
    int                                     _previousSearchFinishFrame;
    bool                                    _searchInProgress;
    double                                  _totalPreviousSearchTime;
    std::vector<MetaPair>                   _previousGoalUnits;
    std::string                             _previousStatus;

    SearchPtr                               _smartSearch;

    BOSS::DFBB_BuildOrderSearchResults      _previousSearchResults;
    BOSS::DFBB_BuildOrderSearchResults      _savedSearchResults;
    BOSS::BuildOrder                        _previousBuildOrder;
	const AKBot::OpponentView& _opponentView;

	BOSS::GameState				            getCurrentState();
	BOSS::GameState				            getStartState();
	
    // functions
	BOSS::DFBB_BuildOrderSearchResults		search(const std::vector<MetaPair> & goalUnits);

	void						            loadOpeningBook();

	std::vector<std::vector<MetaType>>      openingBook;
    const BOSS::RaceID                      getRace() const;

    void                                    logBadSearch();


public:
	
	BOSSManager(const AKBot::OpponentView& opponentView);

	void						update(double timeLimit);
    void                        reset();

    BuildOrder                  getBuildOrder();
    bool                        isSearchInProgress();
	std::string getPreviousStatus() const { return _previousStatus; }
	const std::vector<MetaPair>& getPreviousGoalUnits() const { return _previousGoalUnits; }
	double getTotalPreviousSearchTime() const { return _totalPreviousSearchTime; }
	const BOSS::DFBB_BuildOrderSearchResults& getSavedSearchResults() const { return _savedSearchResults; }

    void                        startNewSearch(const std::vector<MetaPair> & goalUnits, const BuildingManager & buildingManager);
    
	static BOSS::BuildOrderSearchGoal       GetGoal(const std::vector<MetaPair> & goalUnits);	
    static std::vector<MetaType>			GetMetaVector(const BOSS::BuildOrder & buildOrder);
	static BOSS::ActionType					GetActionType(const MetaType & t);
	static MetaType					        GetMetaType(const BOSS::ActionType & a);
};

}