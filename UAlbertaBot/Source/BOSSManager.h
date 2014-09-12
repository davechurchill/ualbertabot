#pragma once

#include "Common.h"
#include "base/WorkerManager.h"
#include "../../BOSS/source/BOSS.h"
#include "StrategyManager.h"

namespace UAlbertaBot
{

#define SEARCH_FRAME_LIMIT 240

typedef boost::shared_ptr<BOSS::DFBB_BuildOrderSmartSearch> SearchPtr;

class BOSSManager
{
    int                                     _previousSearchStartFrame;
    int                                     _previousSearchFinishFrame;
    bool                                    _searchInProgress;

    SearchPtr                               _smartSearch;

    BOSS::DFBB_BuildOrderSearchResults      _previousSearchResults;
    std::vector<BOSS::ActionType>           _previousBuildOrder;

	BOSS::GameState				            getCurrentState();
	BOSS::GameState				            getStartState();
	
    // functions
	BOSS::DFBB_BuildOrderSearchResults		search(const std::vector<MetaPair> & goalUnits);



	void						            loadOpeningBook();

	std::vector<std::vector<MetaType>>      openingBook;
    const BOSS::RaceID                      getRace() const;

	BOSSManager();

public:

	static BOSSManager &	    Instance();

	void						update(double timeLimit);
    void                        reset();

    std::vector<MetaType>       getBuildOrder();
    bool                        isSearchInProgress();

    void                        startNewSearch(const std::vector<MetaPair> & goalUnits);
    
	void						drawSearchInformation(int x, int y);

    
	static BOSS::DFBB_BuildOrderSearchGoal      GetGoal(const std::vector<MetaPair> & goalUnits);	
    static std::vector<MetaType>				GetMetaVector(const std::vector<BOSS::ActionType> & buildOrder);
	static BOSS::ActionType						GetActionType(const MetaType & t);
	static MetaType					            GetMetaType(const BOSS::ActionType & a);

    static std::vector<MetaType>                GetNaiveBuildOrder(const std::vector<MetaPair> & goalUnits);
    static std::vector<MetaType>                GetOptimizedNaiveBuildOrder(const std::vector<MetaPair> & goalUnits);
};

}