#pragma once

#include "Common.h"

namespace UAlbertaBot
{
struct UnitInfo
{
    // we need to store all of this data because if the unit is not visible, we
    // can't reference it from the unit pointer

    int             unitID          = 0;
    int             lastHealth      = 0;
    int             lastShields     = 0;
    BWAPI::Player   player          = nullptr;
    BWAPI::Unit     unit;
    BWAPI::Position lastPosition    = BWAPI::Positions::None;
    BWAPI::UnitType type            = BWAPI::UnitTypes::None;
    bool            completed       = false;

    UnitInfo()
    {

    }

    const bool operator == (BWAPI::Unit unit) const
    {
        return unitID == unit->getID();
    }

    const bool operator == (const UnitInfo & rhs) const
    {
        return (unitID == rhs.unitID);
    }

    const bool operator < (const UnitInfo & rhs) const
    {
        return (unitID < rhs.unitID);
    }
};

typedef std::vector<UnitInfo> UnitInfoVector;
typedef std::map<BWAPI::Unit,UnitInfo> UIMap;

class UnitData
{
    UIMap unitMap;

    std::vector<int> numDeadUnits;
    std::vector<int> numUnits;
    int mineralsLost = 0;
    int gasLost      = 0;
    
    const bool badUnitInfo(const UnitInfo & ui) const;

public:

    UnitData();

    void	updateUnit(BWAPI::Unit unit);
    void	removeUnit(BWAPI::Unit unit);
    void	removeBadUnits();

    int		getGasLost()                                const;
    int		getMineralsLost()                           const;
    int		getNumUnits(BWAPI::UnitType t)              const;
    int		getNumDeadUnits(BWAPI::UnitType t)          const;
    const	std::map<BWAPI::Unit,UnitInfo> & getUnits() const;
};
}