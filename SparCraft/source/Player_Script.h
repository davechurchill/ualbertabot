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
    std::vector<size_t> _numTargeting;
    std::vector<size_t> _damageAssigned;
    std::vector<size_t> _enemyUnitsInAttackRange;

    std::vector<double> _val;
    std::vector<double> _bestVals;

    std::vector< std::vector<double> > _distSq;

    const Position &    getPlayerCenter(const GameState & state, const size_t & playerID);
    void                getEnemyUnitsInAttackRange(const Unit & myUnit, const GameState & state, const std::vector<size_t> & enemyUnits, std::vector<size_t> & unitIDs);
    Action              getPolicyAction        (const GameState & state, const Unit & myUnit, const ScriptPolicy & policy, const std::vector<size_t> & validUnitTargets);
    Position            getPolicyTargetPosition(const GameState & state, const Unit & myUnit, const ScriptPolicy & policy, const std::vector<size_t> & validUnitTargets);
    size_t              getPolicyTargetUnitID  (const GameState & state, const Unit & myUnit, const ScriptPolicy & policy, const std::vector<size_t> & validUnitTargets);
    void                getTargetOperandValue  (const GameState & state, const Unit & myUnit, const ScriptPolicy & policy, const Unit & targetUnit, std::vector<double> & val);
    bool                greater(const std::vector<double> & v1, const std::vector<double> & v2);
    bool                less(const std::vector<double> & v1, const std::vector<double> & v2);

public:

	Player_Script (const size_t & playerID, const ScriptPlayerPolicy & policy);    
	void getMove(const GameState & state, Move & move);

    virtual const std::string & getDescription();

    virtual PlayerPtr clone();

};
}