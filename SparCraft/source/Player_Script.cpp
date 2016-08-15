#include "Player_Script.h"

using namespace SparCraft;

Player_Script::Player_Script (const size_t & playerID, const ScriptPlayerPolicy & policy) 
{
	_playerID = playerID;
}

void Player_Script::getMove(const GameState & state, Move & move)
{
    move.clear();
	const size_t enemy = state.getEnemy(_playerID);


}

PlayerPtr Player_Script::clone()
{
    return PlayerPtr(new Player_Script(*this));
}