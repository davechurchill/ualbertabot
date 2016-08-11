#pragma once

#include "Common.h"
#include "Unit.h"
#include <array>

namespace SparCraft
{

typedef std::vector<Unit>       UnitVector;
typedef std::vector<PlayerID>   UnitIDVector;

class GameStateUnitData 
{
    UnitVector              _allUnits;         
    UnitIDVector            _liveUnitIDs[2]; 
    
    void                    removeUnit(const PlayerID & player, const UnitID & UnitIndex);
    void                    removeUnitByID(const UnitID & unitID);

public:
        
    GameStateUnitData();
    
    const   Unit &          getUnit(const PlayerID & player, const UnitID & unitIndex) const;
            Unit &          getUnit(const PlayerID & player, const UnitID & unitIndex);

    const   Unit &          getUnitByID(const UnitID & id) const;
            Unit &          getUnitByID(const UnitID & id);

    const   UnitID          numUnits(const PlayerID & player) const;

    const   UnitIDVector &  getUnitIDs(const PlayerID & player) const;

    Unit &                  addUnit(const Unit & unit);
    void                    killUnit(const UnitID & unitID);
};

}

