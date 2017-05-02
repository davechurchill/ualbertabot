#pragma once

#include "StrategyManager.h"
#include "WorkerManager.h"
#include "InfoManager.h"
#include "MapTools.h"

namespace UAlbertaBot
{

class UAlbertaBotModule;

namespace Global
{      
    void SetModule(UAlbertaBotModule * module);
        
    StrategyManager & Strategy();
    WorkerManager & Workers();
    InfoManager & Info();
    MapTools & Map();
};
}