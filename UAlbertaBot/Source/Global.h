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
        
    WorkerManager & Workers();
    const MapTools & Map();
	BWAPI::Player getEnemy();
};
}