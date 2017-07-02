#pragma once

#include "StrategyManager.h"
#include "WorkerManager.h"
#include "UnitInfoManager.h"
#include "MapTools.h"
#include "BaseLocationManager.h"

namespace UAlbertaBot
{

class UAlbertaBot_Tournament;

namespace Global
{      
    void SetModule(UAlbertaBot_Tournament * module);
        
    const BaseLocationManager & Bases();
    const StrategyManager & Strategy();
    WorkerManager & Workers();
    const UnitInfoManager & UnitInfo();
    const MapTools & Map();
	BWAPI::Player getEnemy();
};
}