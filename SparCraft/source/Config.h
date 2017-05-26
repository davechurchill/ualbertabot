#pragma once

#include "Common.h"

namespace SparCraft
{

typedef std::pair<TimeType,TimeType> AttackFrameData;

namespace Config
{
    void InitAttackFrames();
    
    namespace Units
    {
        extern int GroundUnitMovePenalty;
        extern int AirUnitMovePenalty;
        extern int UnitRangeAddition;
        extern int UnitMoveAfterAttackBuffer;
        const AttackFrameData & GetAttackFrames(const BWAPI::UnitType & type);
    }
};

}