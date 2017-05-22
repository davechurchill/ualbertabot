#include "Global.h"
#include "UAlbertaBot_Tournament.h"

using namespace UAlbertaBot;

UAlbertaBot_Tournament * uabModule = nullptr;

void Global::SetModule(UAlbertaBot_Tournament * module)
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