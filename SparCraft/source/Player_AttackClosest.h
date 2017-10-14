#pragma once

#include "Common.h"
#include "Player.h"

namespace SparCraft
{
    class Player_AttackClosest;
}

/*----------------------------------------------------------------------
 | Attack Closest Player
 |----------------------------------------------------------------------
 | Chooses an action with following priority:
 | 1) If it can attack, ATTACK closest enemy unit
 | 2) If it cannot attack:
 |    a) If it is in range to attack an enemy, WAIT until can shoot again
 |    b) If it is not in range of enemy, MOVE towards closest
 `----------------------------------------------------------------------*/
class SparCraft::Player_AttackClosest : public SparCraft::Player
{
public:
	Player_AttackClosest (const size_t & playerID);
	void getMove(const GameState & state, Move & move);
    virtual PlayerPtr clone();
};