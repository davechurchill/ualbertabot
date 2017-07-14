#pragma once

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
    WorkerManager & Workers();
    const UnitInfoManager & UnitInfo();
    const MapTools & Map();
	BWAPI::Player getEnemy();
};
}