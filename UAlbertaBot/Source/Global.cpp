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

}

Global & Global::Instance()
{
    static Global instance;
    return instance;
}

void Global::init()
{
    /*m_mapTools              = std::shared_ptr<MapTools>(new MapTools());
    m_baseLocationManager   = std::shared_ptr<BaseLocationManager>(new BaseLocationManager());
    m_informationManager    = std::shared_ptr<InformationManager>(new InformationManager());
    m_strategyManager       = std::shared_ptr<StrategyManager>(new StrategyManager());
    m_workerManager         = std::shared_ptr<WorkerManager>(new WorkerManager());
    m_bossManager           = std::shared_ptr<BOSSManager>(new BOSSManager());
    m_productionManager     = std::shared_ptr<ProductionManager>(new ProductionManager());
    m_buildingManager       = std::shared_ptr<BuildingManager>(new BuildingManager());
    m_scoutManager          = std::shared_ptr<ScoutManager>(new ScoutManager());
    m_buildingPlacer        = std::shared_ptr<BuildingPlacer>(new BuildingPlacer());*/
}

BuildingPlacerManager & Global::BuildingPlacer()
{
    static BuildingPlacerManager instance;
    return instance;
}

MapTools & Global::Map()
{
    static MapTools instance;
    return instance;
}

BaseLocationManager & Global::Bases()
{
    static BaseLocationManager instance;
    return instance;
}

InformationManager & Global::Info()
{
    static InformationManager instance;
    return instance;
}

StrategyManager & Global::Strategy()
{
    static StrategyManager instance;
    return instance;
}

WorkerManager & Global::Workers()
{
    static WorkerManager instance;
    return instance;
}

BOSSManager & Global::BOSS()
{
    static BOSSManager instance;
    return instance;
}

ProductionManager & Global::Production()
{
    static ProductionManager instance;
    return instance;
}

BuildingManager & Global::Buildings()
{
    static BuildingManager instance;
    return instance;
}

ScoutManager & Global::Scout()
{
    static ScoutManager instance;
    return instance;
}



/*
BuildingPlacer & Global::BuildingPlacer() { return *Instance().m_buildingPlacer; }
MapTools & Global::Map() { return *Instance().m_mapTools; }
BaseLocationManager & Global::Bases() { return *Instance().m_baseLocationManager; }
InformationManager & Global::Info() { return *Instance().m_informationManager; }
StrategyManager & Global::Strategy() { return *Instance().m_strategyManager; }
WorkerManager & Global::Workers() { return *Instance().m_workerManager; }
BOSSManager & Global::BOSS() { return *Instance().m_bossManager; }
ProductionManager & Global::Production() { return *Instance().m_productionManager; }
BuildingManager & Global::Buildings() { return *Instance().m_buildingManager; }
ScoutManager & Global::Scout() { return *Instance().m_scoutManager; }
*/