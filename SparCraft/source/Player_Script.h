#pragma once

#include "Common.h"
#include "Player.h"
#include "ScriptPlayerPolicy.h"

namespace SparCraft
{

class Player_Script : public Player
{

    ScriptPlayerPolicy _policy;

public:
	Player_Script (const size_t & playerID, const ScriptPlayerPolicy & policy);
	void getMove(const GameState & state, Move & move);
	virtual PlayerPtr clone();
};
}