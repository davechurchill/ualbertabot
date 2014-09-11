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
	Player_AttackClosest (const IDType & playerID);
	void getMoves(GameState & state, const MoveArray & moves, std::vector<UnitAction> & moveVec);
	IDType getType() { return PlayerModels::AttackClosest; }
};