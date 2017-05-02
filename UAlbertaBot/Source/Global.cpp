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

WorkerManager & Global::Workers()
{
    return uabModule->getWorkerManager();
}

InfoManager & Global::Info()
{
    return uabModule->getInfoManager();
}

MapTools & Global::Map()
{
    return uabModule->getMapTools();
}