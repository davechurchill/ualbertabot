#pragma once

#include "Common.h"
#include "../../BOSS/source/BOSS.h"
#include <memory>
#include "OpponentView.h"
#include "BuildingManager.h"
#include "StrategyConfiguration.h"
#include "BotConfiguration.h"

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
	const BotDebugConfiguration&			_debugConfiguration;
	bool									_hasExceptionDuringSearch;
	bool									_noSolutionFound;
	int										_bossFrameLimit = 160;

    SearchPtr                               _smartSearch;

    BOSS::DFBB_BuildOrderSearchResults      _previousSearchResults;
    BOSS::DFBB_BuildOrderSearchResults      _savedSearchResults;
    BOSS::BuildOrder                        _previousBuildOrder;
	shared_ptr<AKBot::OpponentView> _opponentView;

	BOSS::GameState				            getCurrentState();
	BOSS::GameState				            getStartState();
	
    // functions
	BOSS::DFBB_BuildOrderSearchResults		search(const std::vector<MetaPair> & goalUnits);

	void						            loadOpeningBook();

	std::vector<std::vector<MetaType>>      openingBook;
    const BOSS::RaceID                      getRace() const;

    void                                    logBadSearch();


public:
	
	BOSSManager(
		shared_ptr<AKBot::OpponentView> opponentView,
		const BotDebugConfiguration& debugConfiguration);
	BOSSManager(const BOSSManager&) = delete;

	void						update(double timeLimit, int currentFrame);
    void                        reset();

    BuildOrder                  getBuildOrder();
    bool                        isSearchInProgress();
	std::string getPreviousStatus() const { return _previousStatus; }
	const std::vector<MetaPair>& getPreviousGoalUnits() const { return _previousGoalUnits; }
	double getTotalPreviousSearchTime() const { return _totalPreviousSearchTime; }
	const BOSS::DFBB_BuildOrderSearchResults& getSavedSearchResults() const { return _savedSearchResults; }
	bool hasExceptionDuringSearch() const { return _hasExceptionDuringSearch; }

    void                        startNewSearch(const std::vector<MetaPair> & goalUnits, shared_ptr<BuildingManager> buildingManager, int currentFrame);
    
	static BOSS::BuildOrderSearchGoal       GetGoal(BWAPI::Player self, const std::vector<MetaPair> & goalUnits);
    static std::vector<MetaType>			GetMetaVector(const BOSS::BuildOrder & buildOrder);
	static BOSS::ActionType					GetActionType(const MetaType & t);
	static MetaType					        GetMetaType(const BOSS::ActionType & a);
	void setBOSSSFrameLimit(int value) { _bossFrameLimit = value; }
	bool getNoSolutionFound() const { return _noSolutionFound; }
};

}