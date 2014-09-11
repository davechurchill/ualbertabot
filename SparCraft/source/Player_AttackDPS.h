#pragma once

#include "Common.h"
#include "Player.h"

namespace SparCraft
{
/*----------------------------------------------------------------------
 | Attack HighestDPS Player
 |----------------------------------------------------------------------
 | Chooses an action with following priority:
 | 1) If it can attack, ATTACK highest DPS/HP enemy unit in range
 | 2) If it cannot attack:
 |    a) If it is in range to attack an enemy, WAIT until attack
 |    b) If it is not in range of enemy, MOVE towards closest
 `----------------------------------------------------------------------*/
class Player_AttackDPS : public Player
{
public:
	Player_AttackDPS (const IDType & playerID);
	void getMoves(GameState & state, const MoveArray & moves, std::vector<UnitAction> & moveVec);
	IDType getType() { return PlayerModels::AttackDPS; }
};
}