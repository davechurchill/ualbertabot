#pragma once

#include <Common.h>
#include <BWAPI.h>

namespace UAlbertaBot
{
namespace DebugTools
{      
    void DrawUnitHPBar(const BWAPI::UnitType & type, const BWAPI::Position & pos, const int & hitPoints, const int & shields);
};
}