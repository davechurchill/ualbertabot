#pragma once

#include "Common.h"
#include "MoveArray.h"
#include "Map.h"
#include "Unit.h"
#include "GraphViz.hpp"
#include "GameStateUnitData.h"


#include <memory>
#include <algorithm>

typedef std::shared_ptr<SparCraft::Map> MapPtr;

namespace SparCraft
{
class GameState
{
    std::shared_ptr<Map>    _map;

    GameStateUnitData       _unitData;

    size_t                  _numMovements[2];
    TimeType                _currentTime;

    void                    doAction(const Action & theMove);

    Unit &                  _getUnitByID(const size_t & unitID);
    Unit &                  _getUnit(const size_t & player,const size_t & unitIndex);

public:

    GameState();

    // misc functions
    void                    updateGameTime(const TimeType nextTime);
    bool                    playerDead(const size_t & player)                                       const;
    bool                    gameOver()                                                              const;
    size_t                  getEnemy(const size_t & player)                                         const;
    size_t                  winner()                                                                const;

    // unit data functions
    size_t                  numUnits(const size_t & player)                                         const;

    // Unit functions
    void                    addUnit(const Unit & u);
    const Unit &            getUnitByID(const size_t & unitID)                                      const;
    const Unit &            getUnit(const size_t & player, const size_t & unitIndex)                const;
    const std::vector<Unit> & getAllUnits()                                                         const;
    const std::vector<size_t> & getUnitIDs(const size_t & player)                                   const;

    // game time functions
    void                    setTime(const TimeType & time);
    TimeType                getTime()                                                               const;
    TimeType                getTimeNextUnitCanAct(const size_t & player)                            const;

    // move related functions
    void                    doMove(const Move & moves);
    void                    doMove(const Move & m1, const Move & m2, bool advanceGameTime = true);
    size_t                  getNumMovements(const size_t & player)                                  const;
    size_t                  whoCanMove()                                                            const;
    bool                    bothCanMove()                                                           const;

    // map-related functions
    void                    setMap(std::shared_ptr<Map> map);
    std::shared_ptr<Map>    getMap()                                                                const;
    bool                    isWalkable(const Position & pos)                                        const;
    bool                    isFlyable(const Position & pos)                                         const;
    
};

}

