#include "Global.h"

#include "MapTools.h"
#include "InformationManager.h"
#include "BaseLocationManager.h"
#include "WorkerManager.h"
#include "StrategyManager.h"
#include "BOSSManager.h"
#include "ProductionManager.h"
#include "BuildingManager.h"
#include "ScoutManager.h"
#include "BuildingPlacerManager.h"

using namespace UAlbertaBot;

Global::Global()
{
    init();
}

Global & Global::Instance()
{
    static Global instance;
    return instance;
}

void Global::init()
{
    m_mapTools              = std::shared_ptr<MapTools>();
    m_informationManager    = std::shared_ptr<InformationManager>();
    m_workerManager         = std::shared_ptr<WorkerManager>();
    m_productionManager     = std::shared_ptr<ProductionManager>();
    m_baseLocationManager   = std::shared_ptr<BaseLocationManager>();
    m_strategyManager       = std::shared_ptr<StrategyManager>();
    m_bossManager           = std::shared_ptr<BOSSManager>();
    m_buildingManager       = std::shared_ptr<BuildingManager>();
    m_scoutManager          = std::shared_ptr<ScoutManager>();
    m_buildingPlacer        = std::shared_ptr<BuildingPlacerManager>();
}

void Global::GameStart()
{
    Instance().init();
}

/*
BuildingPlacerManager & Global::BuildingPlacer()
{
    static BuildingPlacerManager instance;
    if (BWAPI::Broodwar->getFrameCount() == 0) { instance = BuildingPlacerManager(); }
    return instance;
}

MapTools & Global::Map()
{
    static MapTools instance;
    if (BWAPI::Broodwar->getFrameCount() == 0) { instance = MapTools(); }
    return instance;
}

BaseLocationManager & Global::Bases()
{
    static BaseLocationManager instance;
    if (BWAPI::Broodwar->getFrameCount() == 0) { instance = BaseLocationManager(); }
    return instance;
}

InformationManager & Global::Info()
{
    static InformationManager instance;
    if (BWAPI::Broodwar->getFrameCount() == 0) { instance = InformationManager(); }
    return instance;
}

StrategyManager & Global::Strategy()
{
    static StrategyManager instance;
    if (BWAPI::Broodwar->getFrameCount() == 0) { instance = StrategyManager(); }
    return instance;
}

WorkerManager & Global::Workers()
{
    static WorkerManager instance;
    if (BWAPI::Broodwar->getFrameCount() == 0) { instance = WorkerManager(); }
    return instance;
}

BOSSManager & Global::BOSS()
{
    static BOSSManager instance;
    if (BWAPI::Broodwar->getFrameCount() == 0) { instance = BuildingPlacerManager(); }
    return instance;
}

ProductionManager & Global::Production()
{
    static ProductionManager instance;
    if (BWAPI::Broodwar->getFrameCount() == 0) { instance = BuildingPlacerManager(); }
    return instance;
}

BuildingManager & Global::Buildings()
{
    static BuildingManager instance;
    if (BWAPI::Broodwar->getFrameCount() == 0) { instance = BuildingPlacerManager(); }
    return instance;
}

ScoutManager & Global::Scout()
{
    static ScoutManager instance;
    return instance;
}
*/

BuildingPlacerManager & Global::BuildingPlacer() 
{ 
    if (!Instance().m_buildingPlacer) 
    { 
        Instance().m_buildingPlacer = std::shared_ptr<BuildingPlacerManager>(new BuildingPlacerManager()); 
    }
    return *Instance().m_buildingPlacer; 
}

MapTools & Global::Map() 
{ 
    if (!Instance().m_mapTools) 
    { 
        Instance().m_mapTools = std::shared_ptr<MapTools>(new MapTools()); 
    }
    return *Instance().m_mapTools; 
}

BaseLocationManager & Global::Bases() 
{ 
    if (!Instance().m_baseLocationManager) 
    { 
        Instance().m_baseLocationManager = std::shared_ptr<BaseLocationManager>(new BaseLocationManager()); 
    }
    return *Instance().m_baseLocationManager; 
}

InformationManager & Global::Info() 
{ 
    if (!Instance().m_informationManager) 
    { 
        Instance().m_informationManager = std::shared_ptr<InformationManager>(new InformationManager()); 
    }
    return *Instance().m_informationManager; 
}

StrategyManager & Global::Strategy() 
{  
    if (!Instance().m_strategyManager) 
    { 
        Instance().m_strategyManager = std::shared_ptr<StrategyManager>(new StrategyManager()); 
    }
    return *Instance().m_strategyManager; 
}

WorkerManager & Global::Workers()
{ 
    if (!Instance().m_workerManager) 
    { 
        Instance().m_workerManager = std::shared_ptr<WorkerManager>(new WorkerManager()); 
    }
    return *Instance().m_workerManager; 
}

BOSSManager & Global::BOSS() 
{ 
    if (!Instance().m_bossManager) 
    { 
        Instance().m_bossManager = std::shared_ptr<BOSSManager>(new BOSSManager()); 
    }
    return *Instance().m_bossManager; 
}

ProductionManager & Global::Production() 
{ 
    if (!Instance().m_productionManager) 
    { 
        Instance().m_productionManager = std::shared_ptr<ProductionManager>(new ProductionManager()); 
    }
    return *Instance().m_productionManager; 
}

BuildingManager & Global::Buildings() 
{ 
    if (!Instance().m_buildingManager) 
    { 
        Instance().m_buildingManager = std::shared_ptr<BuildingManager>(new BuildingManager()); 
    }
    return *Instance().m_buildingManager; 
}

ScoutManager & Global::Scout() 
{ 
    if (!Instance().m_scoutManager) 
    { 
        Instance().m_scoutManager = std::shared_ptr<ScoutManager>(new ScoutManager()); 
    }
    return *Instance().m_scoutManager; 
}
