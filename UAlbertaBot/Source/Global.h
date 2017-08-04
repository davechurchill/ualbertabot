#pragma once

#include "WorkerManager.h"
#include "UAlbertaBot_Tournament.h"

namespace UAlbertaBot
{

namespace Global
{      
    void SetModule(UAlbertaBot_Tournament * module);
        
    WorkerManager & Workers();
	BWAPI::Player getEnemy();
};
}