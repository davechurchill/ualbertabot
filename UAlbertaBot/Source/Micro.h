#pragma once

#include <Common.h>
#include <BWAPI.h>

namespace UAlbertaBot
{
namespace Micro
{      
    void SmartAttackUnit(BWAPI::UnitInterface * attacker, BWAPI::UnitInterface* target);
    void SmartAttackMove(BWAPI::UnitInterface * attacker, const BWAPI::Position & targetPosition);
    void SmartMove(BWAPI::UnitInterface * attacker, const BWAPI::Position & targetPosition);
    void SmartRightClick(BWAPI::UnitInterface * unit, BWAPI::UnitInterface * target);
    void SmartRepair(BWAPI::UnitInterface * unit, BWAPI::UnitInterface * target);

    void drawAPM(int x, int y);
};
}