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
    void SmartKiteTarget(BWAPI::UnitInterface* rangedUnit, BWAPI::UnitInterface* target);
    void MutaDanceTarget(BWAPI::UnitInterface* muta, BWAPI::UnitInterface* target);
    BWAPI::Position GetKiteVector(BWAPI::UnitInterface * unit, BWAPI::UnitInterface * target);

    void Rotate(double &x, double &y, double angle);
    void Normalize(double &x, double &y);

    void drawAPM(int x, int y);
};
}