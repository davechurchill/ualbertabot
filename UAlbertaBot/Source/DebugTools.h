#pragma once

#include <Common.h>
#include <BWAPI/UnitType.h>
#include "ScreenCanvas.h"

namespace UAlbertaBot
{
namespace DebugTools
{      
    void DrawUnitHPBar(AKBot::ScreenCanvas& canvas, const BWAPI::UnitType & type, const BWAPI::Position & pos, const int & hitPoints, const int & shields);
};
}