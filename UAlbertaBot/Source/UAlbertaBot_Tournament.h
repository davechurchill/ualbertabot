#pragma once

#include "BotModule.h"
#include "GameCommander.h"
#include <iostream>
#include <fstream>
#include "HardCodedInfo.h"
#include "Config.h"
#include "AutoObserver.h"
#include "rapidjson\document.h"
#include "Global.h"
#include "BWAPIScreenCanvas.h"
#include "BOSSManager.h"
#include "debug\GameDebug.h"
#include "ProductionManager.h"
#include "CombatCommander.h"
#include "BOSSManager.h"

namespace UAlbertaBot
{

class UAlbertaBot_Tournament : public BotModule
{
	const BotConfiguration& _configuration;
	shared_ptr<MapTools> _mapTools;
	shared_ptr<BaseLocationManager> _baseLocationManager;
	shared_ptr<AutoObserver> _autoObserver;

	shared_ptr<GameCommander>       _gameCommander;
	shared_ptr<UnitInfoManager>     _unitInfoManager;
	shared_ptr<StrategyManager> _strategyManager;
	shared_ptr<CombatCommander> _combatCommander;
	AKBot::BWAPIScreenCanvas _canvas;
	shared_ptr<AKBot::GameDebug> _gameDebug;

	void drawDebugInformation(AKBot::ScreenCanvas& _canvas);
public:

    UAlbertaBot_Tournament(
		const BotConfiguration& configuration,
		shared_ptr<BaseLocationManager> baseLocationManager,
		shared_ptr<AutoObserver> autoObserver,
		shared_ptr<StrategyManager> strategyManager,
		shared_ptr<UnitInfoManager> unitInfoManager,
		shared_ptr<MapTools> mapTools,
		shared_ptr<CombatCommander> combatManager,
		shared_ptr<GameCommander> gameCommander,
		shared_ptr<AKBot::GameDebug> gameDebug);
    ~UAlbertaBot_Tournament();

    void	onStart();
    void	onFrame();
    void	onEnd(bool isWinner);
    void	onUnitDestroy(BWAPI::Unit unit);
    void	onUnitMorph(BWAPI::Unit unit);
    void	onSendText(std::string text);
    void	onUnitCreate(BWAPI::Unit unit);
    void	onUnitComplete(BWAPI::Unit unit);
    void	onUnitShow(BWAPI::Unit unit);
    void	onUnitHide(BWAPI::Unit unit);
    void	onUnitRenegade(BWAPI::Unit unit);

    const shared_ptr<UnitInfoManager> UnitInfo() const;
	AKBot::ScreenCanvas& getCanvas();
};

}