#include "Player_Script.h"

using namespace SparCraft;

Player_Script::Player_Script (const IDType & playerID) 
{
	_playerID = playerID;
}

void Player_Script::getMoves(GameState & state, std::vector<Action> & moveVec)
{
    moveVec.clear();
	IDType enemy(state.getEnemy(_playerID));

	Array<int, Constants::Max_Units> hpRemaining;

	for (IDType u(0); u<state.numUnits(enemy); ++u)
	{
		hpRemaining[u] = state.getUnit(enemy,u).currentHP();
	}


}

PlayerPtr Player_Script::clone()
{
    return PlayerPtr(new Player_Script(*this));
}