#pragma once

#include "Common.h"
#include "Player.h"

namespace SparCraft
{

/*----------------------------------------------------------------------
 | Attack Weakest Player
 |----------------------------------------------------------------------
 | Chooses an action with following priority:
 | 1) If it can attack, ATTACK least hp enemy unit
 | 2) If it cannot attack:
 |    a) If it is in range to attack an enemy, WAIT
 |    b) If it is not in range of enemy, MOVE towards closest
 `----------------------------------------------------------------------*/
class Player_AttackWeakest : public Player
{
public:
	Player_AttackWeakest (const PlayerID & playerID);
	void getMoves(const GameState & state, std::vector<Action> & moveVec);
	virtual PlayerPtr clone();
};
}