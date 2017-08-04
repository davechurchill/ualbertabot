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
    GameCommander       _gameCommander;
    AutoObserver        _autoObserver;
    WorkerManager       _workerManager;
    UnitInfoManager     _unitInfoManager;
	ProductionManager	_productionManager;
	BOSSManager         _bossManager;
    StrategyManager     _strategyManager;
    MapTools            _mapTools;
    BaseLocationManager _baseLocationManager;
	CombatCommander     _combatCommander;
	ScoutManager		_scoutManager;
	AKBot::BWAPIScreenCanvas _canvas;
	const AKBot::OpponentView& _opponentView;

    void drawErrorMessages() const;
	void drawDebugInformation(AKBot::ScreenCanvas& _canvas);
public:

    UAlbertaBot_Tournament(const AKBot::OpponentView& opponentView, const AKBot::Logger& logger);
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
    const MapTools & Map() const;
	AKBot::ScreenCanvas& getCanvas();
	const AKBot::OpponentView& getOpponentView() const  { return _opponentView; };
};

}