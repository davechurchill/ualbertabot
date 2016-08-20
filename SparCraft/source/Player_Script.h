#pragma once

#include "Common.h"
#include "Player.h"
#include "ScriptPlayerPolicy.h"

namespace SparCraft
{

class Player_Script : public Player
{
    size_t              _enemyID;
    ScriptPlayerPolicy  _playerPolicy;
    

    Position            _playerCenters[2];
    bool                _playerCentersCalculated[2];

    const Position & getPlayerCenter(const GameState & state, const size_t & playerID);
    void getEnemyUnitsInAttackRange(const Unit & myUnit, const GameState & state, std::vector<size_t> & unitIDs);
    
    Action   getPolicyAction        (const GameState & state, const Unit & myUnit, const ScriptPolicy & policy, const std::vector<size_t> & validUnitTargets);
    Position getPolicyTargetPosition(const GameState & state, const Unit & myUnit, const ScriptPolicy & policy, const std::vector<size_t> & validUnitTargets);
    size_t   getPolicyTargetUnitID  (const GameState & state, const Unit & myUnit, const ScriptPolicy & policy, const std::vector<size_t> & validUnitTargets);
    double   getTargetOperandValue  (const GameState & state, const Unit & myUnit, const ScriptPolicy & policy, const Unit & targetUnit);

public:

	Player_Script (const size_t & playerID, const ScriptPlayerPolicy & policy);    
	void getMove(const GameState & state, Move & move);

    virtual PlayerPtr clone();

};
}