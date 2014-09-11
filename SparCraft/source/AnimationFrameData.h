#pragma once

#include "Common.h"

namespace SparCraft
{

typedef std::pair<TimeType,TimeType> AttackFrameData;

class AnimationFrameData
{
    static std::vector<AttackFrameData> attackFrameData;

public:

    static void init();
    static const AttackFrameData & getAttackFrames(const BWAPI::UnitType & type);
};

}