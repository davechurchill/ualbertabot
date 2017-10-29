#pragma once

#include "Common.h"
#include <BWAPI/Player.h>
#include "BuildOrder.h"
#include "Strategy.h"
#include "UnitInfoManager.h"
#include "BaseLocationManager.h"
#include "Logger.h"
#include "BotConfiguration.h"

namespace UAlbertaBot
{

class StrategyManager
{
    std::map<std::string,Strategy> _strategies;
    int                             _totalGamesPlayed;
    BuildOrder                      _emptyBuildOrder;
	shared_ptr<UnitInfoManager>			_unitInfo;
	std::string						_strategyName;
	shared_ptr<AKBot::OpponentView> _opponentView;
	shared_ptr<BaseLocationManager> _bases;
	std::shared_ptr<AKBot::Logger> _logger;
	BotStrategyConfiguration& _strategyConfiguration;

    void        writeResults();
    const int   getScore(BWAPI::Player player) const;
    const bool  shouldExpandNow(int currentFrame) const;
    const MetaPairVector getProtossBuildOrderGoal(int currentFrame) const;
    const MetaPairVector getTerranBuildOrderGoal(int currentFrame) const;
    const MetaPairVector getZergBuildOrderGoal(int currentFrame) const;

public:

    StrategyManager(
		shared_ptr<AKBot::OpponentView> opponentView,
		shared_ptr<UnitInfoManager> unitInfo,
		shared_ptr<BaseLocationManager> bases,
		shared_ptr<AKBot::Logger> logger,
		BotStrategyConfiguration& strategyConfiguration);

    void update();
    void onEnd(const bool isWinner);
    void addStrategy(const std::string & name,Strategy & strategy);
    void setLearnedStrategy();
	void setPreferredStrategy(std::string strategy);
    void readResults();
	const std::string& getPreferredStrategy() const;
    const MetaPairVector getBuildOrderGoal(int currentFrame) const;
    const BuildOrder & getOpeningBookBuildOrder() const;
};
}