#pragma once

#include "MicroManager.h"

namespace UAlbertaBot
{
class MeleeManager : public MicroManager
{

public:

    MeleeManager();
    void executeMicro(const BWAPI::Unitset & targets);
    void assignTargetsNew(const BWAPI::Unitset & targets);
    void assignTargetsOld(const BWAPI::Unitset & targets);

    int getAttackPriority(BWAPI::Unit attacker, BWAPI::Unit unit);
    bool meleeUnitShouldRetreat(BWAPI::Unit meleeUnit, const BWAPI::Unitset & targets);
    BWAPI::Unit closestMeleeUnit(BWAPI::Unit target, const BWAPI::Unitset & meleeUnitToAssign);
    BWAPI::Unit getTarget(BWAPI::Unit meleeUnit, const BWAPI::Unitset & targets);
    std::pair<BWAPI::Unit, BWAPI::Unit> findClosestUnitPair(const BWAPI::Unitset & attackers, const BWAPI::Unitset & targets);
};
}