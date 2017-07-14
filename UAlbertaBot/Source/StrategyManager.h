#pragma once

#include "Common.h"
#include <BWAPI/Player.h>
#include "BuildOrder.h"
#include "Strategy.h"
#include "UnitInfoManager.h"

namespace UAlbertaBot
{

class StrategyManager
{
    std::map<std::string,Strategy> _strategies;
    int                             _totalGamesPlayed;
    BuildOrder                      _emptyBuildOrder;
	const UnitInfoManager&			_unitInfo;
	std::string						_strategyName;
	const AKBot::OpponentView& _opponentView;

    void        writeResults();
    const int   getScore(BWAPI::Player player) const;
    const bool  shouldExpandNow() const;
    const MetaPairVector getProtossBuildOrderGoal() const;
    const MetaPairVector getTerranBuildOrderGoal() const;
    const MetaPairVector getZergBuildOrderGoal() const;

public:

    StrategyManager(std::string strategyName, const AKBot::OpponentView& opponentView, const UnitInfoManager & unitInfo);

    void update();
    void onEnd(const bool isWinner);
    void addStrategy(const std::string & name,Strategy & strategy);
    void setLearnedStrategy();
	void setPreferredStrategy(std::string strategy);
    void readResults();
    const MetaPairVector getBuildOrderGoal() const;
    const BuildOrder & getOpeningBookBuildOrder() const;
};
}