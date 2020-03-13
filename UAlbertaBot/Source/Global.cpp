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
    reset(m_scoutManager);
}

void Global::GameStart()
{
    Instance().init();
}

MapTools &              Global::Map()            { return *get(Instance().m_mapTools);            }
BaseLocationManager &   Global::Bases()          { return *get(Instance().m_baseLocationManager); }
InformationManager &    Global::Info()           { return *get(Instance().m_informationManager);  }
StrategyManager &       Global::Strategy()       { return *get(Instance().m_strategyManager);     }
WorkerManager &         Global::Workers()        { return *get(Instance().m_workerManager);       }
ProductionManager &     Global::Production()     { return *get(Instance().m_productionManager);   }
ScoutManager &          Global::Scout()          { return *get(Instance().m_scoutManager);        }