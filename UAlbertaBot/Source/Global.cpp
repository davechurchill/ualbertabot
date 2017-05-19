#include "Global.h"
#include "UAlbertaBotModule.h"

using namespace UAlbertaBot;

UAlbertaBotModule * uabModule = nullptr;

void Global::SetModule(UAlbertaBotModule * module)
{
    uabModule = module;
}

const StrategyManager & Global::Strategy()
{
    return uabModule->Strategy();
}

const BaseLocationManager & Global::Bases()
{
    return uabModule->Bases();
}

WorkerManager & Global::Workers()
{
    return uabModule->Workers();
}

const UnitInfoManager & Global::UnitInfo()
{
    return uabModule->UnitInfo();
}

const MapTools & Global::Map()
{
    return uabModule->Map();
}