#pragma once

#include "Common.h"
#include "Unit.h"
#include <array>

namespace SparCraft
{

class GameStateUnitData 
{
    std::vector<Unit>       _allUnits;          // vector where all Unit objects live
    std::vector<size_t>     _liveUnitIDs[2];    // vectors of IDs of live units for each player, ID indexes into _allUnits
    
    void                    removeUnit(const size_t & player, const size_t & UnitIndex);
    void                    removeUnitByID(const size_t & unitID);

public:
        
    GameStateUnitData();
    
    const Unit &            getUnit(const size_t & player, const size_t & unitIndex) const;
          Unit &            getUnit(const size_t & player, const size_t & unitIndex);

    const Unit &            getUnitByID(const size_t & id) const;
          Unit &            getUnitByID(const size_t & id);

    const size_t            numUnits(const size_t & player) const;

    const std::vector<Unit> & getAllUnits() const;
    const std::vector<size_t> & getUnitIDs(const size_t & player) const;

    Unit &                  addUnit(const Unit & unit);
    void                    killUnit(const size_t & unitID);
};

}

