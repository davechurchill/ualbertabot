#pragma once

#include "StrategyManager.h"
#include "WorkerManager.h"
#include "UnitInfoManager.h"
#include "MapTools.h"
#include "BaseLocationManager.h"

namespace UAlbertaBot
{

class UAlbertaBotModule;

namespace Global
{      
    void SetModule(UAlbertaBotModule * module);
        
    BaseLocationManager & Bases();
    StrategyManager & Strategy();
    WorkerManager & Workers();
    UnitInfoManager & UnitInfo();
    MapTools & Map();
};
}