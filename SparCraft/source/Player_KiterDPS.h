#pragma once

#include "Common.h"
#include "Player.h"

namespace SparCraft
{
/*----------------------------------------------------------------------
 | Kiter DPS Player
 |----------------------------------------------------------------------
 | Chooses an action with following priority:
 | 1) If it can attack, ATTACK highest DPS/HP enemy unit in range
 | 2) If it cannot attack:
 |    a) If it is in range to attack an enemy, move away from closest one
 |    b) If it is not in range of enemy, MOVE towards closest one
 `----------------------------------------------------------------------*/
class Player_KiterDPS : public Player
{
public:
	Player_KiterDPS (const IDType & playerID);
	void getMoves(GameState & state, const MoveArray & moves, std::vector<UnitAction> & moveVec);
	IDType getType() { return PlayerModels::KiterDPS; }
};
}