#pragma once

#include "Common.h"
#include <BWAPI/Unit.h>
#include <BWAPI/Player.h>

namespace UAlbertaBot
{
struct UnitInfo
{
    // we need to store all of this data because if the unit is not visible, we
    // can't reference it from the unit pointer

    int             unitID;
    int             lastHealth;
    int             lastShields;
    BWAPI::Player   player;
    BWAPI::Unit     unit;
    BWAPI::Position lastPosition;
    BWAPI::UnitType type;
    bool            completed;

    UnitInfo()
        : unitID(0)
        , lastHealth(0)
        , player(nullptr)
        , unit(nullptr)
        , lastPosition(BWAPI::Positions::None)
        , type(BWAPI::UnitTypes::None)
        , completed(false)
    {

    }

	UnitInfo(BWAPI::Unit unit)
		: unitID(unit->getID())
		, lastHealth(unit->getHitPoints())
		, lastShields(unit->getShields())
		, player(unit->getPlayer())
		, unit(unit)
		, lastPosition(unit->getPosition())
		, type(unit->getType())
		, completed(unit->isCompleted())

	{

	}

    const bool operator == (BWAPI::Unit target) const
    {
        return unitID == target->getID();
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

class UnitData
{
    std::map<int, UnitInfo> _unitMap; // indexed by BWAPI::Unit ID

    const bool badUnitInfo(const UnitInfo & ui) const;

    std::vector<int> _numDeadUnits;
    std::vector<int> _numUnits;

    int _mineralsLost;
    int	_gasLost;

public:

    UnitData();

    void	updateUnit(BWAPI::Unit unit);
    void	removeUnit(BWAPI::Unit unit);
    void	removeBadUnits();

    int		getGasLost()                                const;
    int		getMineralsLost()                           const;
    int		getNumUnits(BWAPI::UnitType t)              const;
    int		getNumDeadUnits(BWAPI::UnitType t)          const;
    const	std::map<int, UnitInfo> & getUnitInfoMap() const;
};
}