#pragma once

#include "Common.h"
#include "MoveArray.h"
#include "Hash.h"
#include "Map.hpp"
#include "Unit.h"
#include "GraphViz.hpp"
#include "Array.hpp"
#include "GameStateUnitData.h"


#include <memory>
#include <algorithm>

typedef std::shared_ptr<SparCraft::Map> MapPtr;

namespace SparCraft
{
class GameState
{
    Map *                   _map;

    GameStateUnitData       _unitData;

    size_t                  _numMovements[2];
    TimeType                _currentTime;
    TimeType                _sameHPFrames;

    void                    doAction(const Action & theMove);

    Unit &                  _getUnitByID(const size_t & unitID);
    Unit &                  _getUnit(const size_t & player,const size_t & unitIndex);

public:

    GameState();
    GameState(const std::string & filename);

    // misc functions
    void                    updateGameTime();
    bool                    playerDead(const size_t & player)                                       const;
    bool                    isTerminal()                                                            const;
    size_t                  getEnemy(const size_t & player)                                         const;

    // unit data functions
    size_t                  numUnits(const size_t & player)                                         const;
    size_t                  numTotalUnits(const size_t & player)                                    const;
    size_t                  prevNumUnits(const size_t & player)                                     const;

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
    void                    doMove(const Move & m1,const Move & m2);
    size_t                  getNumMovements(const size_t & player)                                  const;
    size_t                  whoCanMove()                                                            const;
    bool                    bothCanMove()                                                           const;

    // map-related functions
    void                    setMap(Map * map);
    Map *                   getMap()                                                                const;
    bool                    isWalkable(const Position & pos)                                        const;
    bool                    isFlyable(const Position & pos)                                         const;

    // hashing functions
    HashType                calculateHash(const size_t & hashNum)                                   const;
    
};

}

