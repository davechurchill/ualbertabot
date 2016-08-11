#include "Player.h"

using namespace SparCraft;

void Player::getMoves(const GameState & state, std::vector<Action> & moveVec)
{
	SPARCRAFT_ASSERT(false, "Base class Player constructor called");
}

const IDType Player::ID() 
{ 
	return _playerID; 
}

std::string Player::getDescription() 
{ 
    return "Player Type:    ERROR"; 
}

void Player::setDescription(const std::string & desc) 
{ 
    _description = desc; 
}

PlayerPtr Player::clone() 
{ 
    return PlayerPtr(new Player(*this));
}
