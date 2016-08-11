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
    Map *                                                           _map;               

    GameStateUnitData                                               _unitData;

    std::vector<Unit> _units[Constants::Num_Players];
    std::vector<int>  _unitIndex[Constants::Num_Players];

    Array2D<Unit, Constants::Num_Players, Constants::Max_Units>     _units2;             
    Array2D<int, Constants::Num_Players, Constants::Max_Units>      _unitIndex2;        
    Array<Unit, 1>                                                  _neutralUnits;

    Array<UnitCountType, Constants::Num_Players>                    _numUnits;
    Array<UnitCountType, Constants::Num_Players>                    _prevNumUnits;

    Array<float, Constants::Num_Players>                            _totalLTD;
    Array<float, Constants::Num_Players>                            _totalSumSQRT;

    Array<int, Constants::Num_Players>                              _numMovements;
    Array<int, Constants::Num_Players>                              _prevHPSum;
	
    TimeType                                                        _currentTime;
    size_t                                                          _maxUnits;
    TimeType                                                        _sameHPFrames;

    // checks to see if the unit array is full before adding a unit to the state
    const bool              checkFull(const PlayerID & player)                                        const;
    const bool              checkUniqueUnitIDs()                                                    const;

    void                    performAction(const Action & theMove);

public:

    GameState();
    GameState(const std::string & filename);

	// misc functions
    void                    finishedMoving();
    void                    updateGameTime();
    const bool              playerDead(const PlayerID & player)                                       const;
    const bool              isTerminal()                                                            const;

    // unit data functions
    const size_t            numUnits(const PlayerID & player)                                         const;
    const size_t            prevNumUnits(const PlayerID & player)                                     const;
    const size_t            numNeutralUnits()                                                       const;
    const size_t            closestEnemyUnitDistance(const Unit & unit)                             const;

    // Unit functions
    void                    sortUnits();
    void                    addUnit(const Unit & u);
    void                    addUnit(const BWAPI::UnitType unitType, const PlayerID playerID, const Position & pos);
    void                    addUnitWithID(const Unit & u);
    void                    addNeutralUnit(const Unit & unit);
    const Unit &            getUnit(const PlayerID & player, const UnitCountType & unitIndex)         const;
    const Unit &            getUnitByID(const PlayerID & unitID)                                      const;
          Unit &            getUnit(const PlayerID & player, const UnitCountType & unitIndex);
    const Unit &            getUnitByID(const PlayerID & player, const PlayerID & unitID)               const;
          Unit &            getUnitByID(const PlayerID & player, const PlayerID & unitID);
    const Unit &            getClosestEnemyUnit(const PlayerID & player, const PlayerID & unitIndex, bool checkCloaked=false) const;
    const Unit &            getClosestOurUnit(const PlayerID & player, const PlayerID & unitIndex)      const;
    const Unit &            getUnitDirect(const PlayerID & player, const PlayerID & unit)               const;
    const Unit &            getNeutralUnit(const size_t & u)                                        const;
    
    // game time functions
    void                    setTime(const TimeType & time);
    const TimeType          getTime()                                                               const;

    // evaluation functions
    const StateEvalScore    eval(   const PlayerID & player, const PlayerID & evalMethod, 
                                    const PlayerID p1Script = PlayerModels::NOKDPS,
                                    const PlayerID p2Script = PlayerModels::NOKDPS)                   const;
    const ScoreType         evalLTD(const PlayerID & player)                                        const;
    const ScoreType         evalLTD2(const PlayerID & player)                                       const;
    const ScoreType         LTD(const PlayerID & player)                                            const;
    const ScoreType         LTD2(const PlayerID & player)                                           const;
    const StateEvalScore    evalSim(const PlayerID & player, const PlayerID & p1, const PlayerID & p2)    const;
    const PlayerID            getEnemy(const PlayerID & player)                                         const;

    // unit hitpoint calculations, needed for LTD2 evaluation
    void                    calculateStartingHealth();
    void                    setTotalLTD(const float & p1, const float & p2);
    void                    setTotalLTD2(const float & p1, const float & p2);
    const float &           getTotalLTD(const PlayerID & player)                                    const;
    const float &           getTotalLTD2(const PlayerID & player)                                   const;

    // move related functions
    void                    generateMoves(MoveArray & moves, const PlayerID & playerIndex)            const;
    void                    makeMoves(const std::vector<Action> & moves);
    const int &             getNumMovements(const PlayerID & player)                                  const;
    const PlayerID            whoCanMove()                                                            const;
    const bool              bothCanMove()                                                           const;
		  
    // map-related functions
    void                    setMap(Map * map);
    Map *                   getMap()                                                                const;
    const bool              isWalkable(const Position & pos)                                        const;
    const bool              isFlyable(const Position & pos)                                         const;

    // hashing functions
    const HashType          calculateHash(const size_t & hashNum)                                   const;

    // state i/o functions
    void                    print(int indent = 0) const;
	std::string             toString() const;
    std::string             toStringCompact() const;
    void                    write(const std::string & filename)                                     const;
    void                    read(const std::string & filename);
};

}

