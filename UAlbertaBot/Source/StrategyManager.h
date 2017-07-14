#pragma once

#include "Common.h"
#include "BuildOrder.h"
#include "Strategy.h"
#include <BWAPI/Player.h>

namespace UAlbertaBot
{

class StrategyManager
{
    std::map<std::string,Strategy> _strategies;
    int                             _totalGamesPlayed;
    BuildOrder                      _emptyBuildOrder;

    void        writeResults();
    const int   getScore(BWAPI::Player player) const;
    const bool  shouldExpandNow() const;
    const MetaPairVector getProtossBuildOrderGoal() const;
    const MetaPairVector getTerranBuildOrderGoal() const;
    const MetaPairVector getZergBuildOrderGoal() const;

public:

    StrategyManager();

    void update();
    void onEnd(const bool isWinner);
    void addStrategy(const std::string & name,Strategy & strategy);
    void setLearnedStrategy();
    void readResults();
    const MetaPairVector getBuildOrderGoal() const;
    const BuildOrder & getOpeningBookBuildOrder() const;
};
}