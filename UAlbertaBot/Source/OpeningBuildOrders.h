#pragma once

#include "Common.h"
#include "BWAPI.h"
#include "MetaType.h"

namespace UAlbertaBot
{
typedef std::vector<MetaType> MetaVector;

namespace OpeningBuildOrders
{

    MetaVector ProtossZealotRush();
    MetaVector ProtossDarkTemplarRush();
    MetaVector ProtossDragoonRush();
    MetaVector TerranMarineRush();
    MetaVector ZergLairRush();
};
}