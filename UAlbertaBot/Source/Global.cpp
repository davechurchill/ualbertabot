#include "Global.h"
#include "UAlbertaBotModule.h"

using namespace UAlbertaBot;

UAlbertaBotModule * uabModule = nullptr;

void Global::SetModule(UAlbertaBotModule * module)
{
    uabModule = module;
}

StrategyManager & Global::Strategy()
{
    return uabModule->getStrategyManager();
}

BaseLocationManager & Global::Bases()
{
    return uabModule->getBaseLocationManager();
}

WorkerManager & Global::Workers()
{
    return uabModule->getWorkerManager();
}

UnitInfoManager & Global::UnitInfo()
{
    return uabModule->getUnitInfoManager();
}

MapTools & Global::Map()
{
    return uabModule->getMapTools();
}