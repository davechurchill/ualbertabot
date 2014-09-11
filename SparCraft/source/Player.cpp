#include "Player.h"

using namespace SparCraft;


void Player::getMoves(GameState & state, const MoveArray & moves, std::vector<UnitAction> & moveVec)
{
	// not implemented
}

const IDType Player::ID() 
{ 
	return _playerID; 
}

void Player::setID(const IDType & playerID)
{
	_playerID = playerID;
}
