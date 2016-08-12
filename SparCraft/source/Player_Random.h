#pragma once

#include "Common.h"
#include "Player.h"
#include "Random.hpp"


namespace SparCraft
{
/*----------------------------------------------------------------------
 | Random Player
 |----------------------------------------------------------------------
 | Chooses a random legal move per unit and implements it
 `----------------------------------------------------------------------*/
class Player_Random : public Player
{
	RandomInt rand;
public:
	Player_Random (const size_t & playerID);
	void getMoves(const GameState & state, std::vector<Action> & moveVec);
	virtual PlayerPtr clone();
};
}