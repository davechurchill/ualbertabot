#pragma once

#include "Common.h"
#include "Player.h"
#include "ScriptPlayerPolicy.h"

namespace SparCraft
{

class Player_Script : public Player
{

    ScriptPlayerPolicy  _policy;

    Position            _playerCenters[2];
    bool                _playerCentersCalculated[2];

    const Position & getPlayerCenter(const GameState & state, const size_t & playerID);
    void getEnemyUnitsInAttackRange(const Unit & myUnit, const GameState & state, std::vector<size_t> & unitIDs);

public:

	Player_Script (const size_t & playerID, const ScriptPlayerPolicy & policy);
    
	void getMove(const GameState & state, Move & move);
	
    virtual PlayerPtr clone();

};
}