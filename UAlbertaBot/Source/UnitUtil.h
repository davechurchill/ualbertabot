#pragma once

#include <Common.h>
#include <BWAPI.h>

namespace UAlbertaBot
{
namespace UnitUtil
{      
    bool IsCombatUnit(BWAPI::UnitInterface * unit);
    bool IsValidUnit(BWAPI::UnitInterface * unit);
    bool CanAttackAir(BWAPI::UnitInterface * unit);
    bool CanAttackGround(BWAPI::UnitInterface * unit);
};
}