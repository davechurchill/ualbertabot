#pragma once

#include "Common.h"
#include "../../BOSS/source/BOSS.h"
#include <memory>
#include "MetaType.h"

namespace UAlbertaBot
{
    
typedef std::shared_ptr<BOSS::DFBB_BuildOrderSmartSearch> SearchPtr;
class BuildOrder;

class BOSSManager
{
    friend class Global;
    
    int         m_previousSearchStartFrame  = 0;
    int         m_savedSearchStartFrame     = 0;
    int         m_previousSearchFinishFrame = 0;
    bool        m_searchInProgress          = false;
    double      m_totalPreviousSearchTime   = 0;
    std::string m_previousStatus            = "No Searches";
    SearchPtr   m_smartSearch;
    
    std::vector<MetaPair>                   m_previousGoalUnits;
    BOSS::DFBB_BuildOrderSearchResults      m_previousSearchResults;
    BOSS::DFBB_BuildOrderSearchResults      m_savedSearchResults;
    BOSS::BuildOrder                        m_previousBuildOrder;

	BOSS::GameState				            getStartState();
    const BOSS::RaceID                      getRace() const;
    void                                    logBadSearch();

public:
    
	BOSSManager();

	void update(double timeLimit);
    void reset();
    void startNewSearch(const std::vector<MetaPair> & goalUnits);
	void drawSearchInformation(int x, int y);
    void drawStateInformation(int x, int y);
    bool isSearchInProgress();

    BuildOrder getBuildOrder();

	static BOSS::BuildOrderSearchGoal   GetGoal(const std::vector<MetaPair> & goalUnits);	
    static std::vector<MetaType>        GetMetaVector(const BOSS::BuildOrder & buildOrder);
	static BOSS::ActionType             GetActionType(const MetaType & t);
	static MetaType                     GetMetaType(const BOSS::ActionType & a);
};

}