#pragma once

#include "Common.h"

namespace SparCraft
{

typedef std::pair<size_t,size_t> AttackFrameData;

class AnimationFrameData
{
    static std::vector<AttackFrameData> attackFrameData;

public:

    static void init();
    static const AttackFrameData & getAttackFrames(const BWAPI::UnitType & type);
};

}