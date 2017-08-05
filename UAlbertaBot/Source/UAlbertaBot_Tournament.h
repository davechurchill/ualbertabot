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

namespace UAlbertaBot
{

class UAlbertaBot_Tournament : public BotModule
{
	shared_ptr<MapTools> _mapTools;
    GameCommander       _gameCommander;
    AutoObserver        _autoObserver;
    UnitInfoManager     _unitInfoManager;
	ProductionManager	_productionManager;
	BOSSManager         _bossManager;
    StrategyManager     _strategyManager;
	CombatCommander     _combatCommander;
	AKBot::BWAPIScreenCanvas _canvas;
    shared_ptr<WorkerManager> _workerManager;
	shared_ptr<BaseLocationManager> _baseLocationManager;
	shared_ptr<ScoutManager> _scoutManager;
	shared_ptr<AKBot::OpponentView> _opponentView;

	void drawDebugInformation(AKBot::ScreenCanvas& _canvas);
public:

    explicit UAlbertaBot_Tournament(
		shared_ptr<AKBot::OpponentView> opponentView,
		shared_ptr<BaseLocationManager> baseLocationManager,
		shared_ptr<MapTools> mapTools,
		shared_ptr<WorkerManager> workerManager,
		shared_ptr<ScoutManager> scoutManager,
		shared_ptr<AKBot::Logger> logger);
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

    const UnitInfoManager & UnitInfo() const;
	AKBot::ScreenCanvas& getCanvas();
	const std::shared_ptr<AKBot::OpponentView> getOpponentView() const  { return _opponentView; };
};

}