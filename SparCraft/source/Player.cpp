#include "Player.h"

using namespace SparCraft;

void Player::getMove(const GameState & state, Move & move)
{
	SPARCRAFT_ASSERT(false, "Base class Player constructor called");
}

const size_t Player::ID() 
{ 
	return _playerID; 
}

std::string Player::getDescription() 
{ 
    return _description;
}

void Player::setDescription(const std::string & desc) 
{ 
    _description = desc; 
}

PlayerPtr Player::clone() 
{ 
    return PlayerPtr(new Player(*this));
}
