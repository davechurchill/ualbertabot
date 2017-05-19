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
        
    const BaseLocationManager & Bases();
    const StrategyManager & Strategy();
    WorkerManager & Workers();
    const UnitInfoManager & UnitInfo();
    const MapTools & Map();
};
}