#pragma once

#include <BWAPI.h>
#include "BotModule.h"
#include "GameCommander.h"
#include <iostream>
#include <fstream>
#include "Logger.h"
#include "HardCodedInfo.h"
#include "Config.h"
#include "AutoObserver.h"
#include "rapidjson\document.h"
#include "Global.h"

namespace UAlbertaBot
{

class UAlbertaBot_Tournament : public BotModule
{
    GameCommander       _gameCommander;
    AutoObserver        _autoObserver;
    WorkerManager       _workerManager;
    UnitInfoManager     _unitInfoManager;
    StrategyManager     _strategyManager;
    MapTools            _mapTools;
    BaseLocationManager _baseLocationManager;

    void drawErrorMessages() const;

public:

    UAlbertaBot_Tournament();
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

    WorkerManager & Workers();
    const UnitInfoManager & UnitInfo() const;
    const StrategyManager & Strategy() const;
    const BaseLocationManager & Bases() const;
    const MapTools & Map() const;
};

}