#pragma once

#include "Common.h"
#include "BuildOrder.h"

namespace UAlbertaBot
{

struct Strategy
{
    std::string m_name          = "None";
    BWAPI::Race m_race          = BWAPI::Races::None;
    int         m_wins          = 0;
    int         m_losses        = 0;
    BuildOrder  m_buildOrder;

    Strategy()
    {

    }

    Strategy(const std::string & name, const BWAPI::Race & race, const BuildOrder & buildOrder)
        : m_name(name)
        , m_race(race)
        , m_wins(0)
        , m_losses(0)
        , m_buildOrder(buildOrder)
    {

    }
};

class StrategyManager
{
    friend class Global;

    StrategyManager();

    BWAPI::Race					    m_selfRace;
    BWAPI::Race					    m_enemyRace;
    std::map<std::string, Strategy> m_strategies;
    int                             m_totalGamesPlayed = 0;
    BuildOrder                      m_emptyBuildOrder;

    void                    writeResults();
    const	int             getScore(BWAPI::Player player) const;
    const	bool            shouldExpandNow() const;
    const	MetaPairVector  getProtossBuildOrderGoal() const;
    const	MetaPairVector  getTerranBuildOrderGoal() const;
    const	MetaPairVector  getZergBuildOrderGoal() const;

public:

    void                    onEnd(const bool isWinner);
    void                    addStrategy(const std::string & name, Strategy & strategy);
    void                    setLearnedStrategy();
    void	                readResults();
    const	MetaPairVector  getBuildOrderGoal();
    const	BuildOrder &    getOpeningBookBuildOrder() const;
};
}