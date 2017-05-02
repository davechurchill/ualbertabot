#pragma once

#include <BWAPI.h>
#include "GameCommander.h"
#include <iostream>
#include <fstream>
#include "Logger.h"
#include "HardCodedInfo.h"
#include "Config.h"
#include "AutoObserver.h"
#include "rapidjson\document.h"
#include "Global.h"
#include "MapTools.h"
#include "InfoManager.h"

namespace UAlbertaBot
{

class UAlbertaBotModule
{
	GameCommander   _gameCommander;
    AutoObserver    _autoObserver;
    WorkerManager   _workerManager;
    InfoManager     _infoManager;
    StrategyManager _strategyManager;
    MapTools        _mapTools;

public:

            UAlbertaBotModule();
            ~UAlbertaBotModule();

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

    WorkerManager & getWorkerManager();
    InfoManager & getInfoManager();
    StrategyManager & getStrategyManager();
    MapTools & getMapTools();
};

}