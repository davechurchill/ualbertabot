#pragma once

#include "MicroManager.h"

namespace UAlbertaBot
{
class DetectorManager : public MicroManager
{
    std::map<BWAPI::Unit, bool>	m_cloakedUnitMap;
    BWAPI::Unit unitClosestToEnemy = nullptr;

    bool isAssigned(BWAPI::Unit unit);

public:

    DetectorManager();

    void setUnitClosestToEnemy(BWAPI::Unit unit) { unitClosestToEnemy = unit; }
    void executeMicro(const BWAPI::Unitset & targets);

    BWAPI::Unit closestCloakedUnit(const BWAPI::Unitset & cloakedUnits, BWAPI::Unit detectorUnit);
};
}