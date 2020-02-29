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
    reset(m_mapTools);
    reset(m_informationManager);
    reset(m_workerManager);
    reset(m_productionManager);
    reset(m_baseLocationManager);
    reset(m_strategyManager);
    reset(m_bossManager);
    reset(m_buildingManager);
    reset(m_scoutManager);
    reset(m_buildingPlacer);
}

void Global::GameStart()
{
    Instance().init();
}

BuildingPlacerManager & Global::BuildingPlacer() { return *get(Instance().m_buildingPlacer);      }
MapTools &              Global::Map()            { return *get(Instance().m_mapTools);            }
BaseLocationManager &   Global::Bases()          { return *get(Instance().m_baseLocationManager); }
InformationManager &    Global::Info()           { return *get(Instance().m_informationManager);  }
StrategyManager &       Global::Strategy()       { return *get(Instance().m_strategyManager);     }
WorkerManager &         Global::Workers()        { return *get(Instance().m_workerManager);       }
BOSSManager &           Global::BOSS()           { return *get(Instance().m_bossManager);         }
ProductionManager &     Global::Production()     { return *get(Instance().m_productionManager);   }
BuildingManager &       Global::Buildings()      { return *get(Instance().m_buildingManager);     }
ScoutManager &          Global::Scout()          { return *get(Instance().m_scoutManager);        }







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
