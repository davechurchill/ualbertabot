#include "Player.h"

using namespace SparCraft;

Player::Player()
    : _previousResultTime (0)
    , _maxResultTime      (0)
    , _totalResultTime    (0)
    , _calls              (0)
{

}

void Player::getMove(const GameState & state, Move & move)
{
	SPARCRAFT_ASSERT(false, "Base class Player move called");
}

const size_t Player::ID() 
{ 
	return _playerID; 
}

const std::string & Player::getDescription() 
{ 
    return _description;
}

const std::string & Player::getName()
{
    return _name;
}

void Player::setName(const std::string & name) 
{ 
    _name = name; 
}

PlayerPtr Player::clone() 
{ 
    return PlayerPtr(new Player(*this));
}

void Player::startTimer()
{
    _timer.start();
    _calls++;
}

void Player::stopTimer()
{
    _previousResultTime = _timer.getElapsedTimeInMilliSec();
    _maxResultTime = std::max(_maxResultTime, _previousResultTime);
    _totalResultTime += _previousResultTime;
}


const std::string & Player::getResultString()
{
    std::stringstream ss;
    ss << "Num Calls: " << _calls << "\n";
    ss << "Prev Time: " << _previousResultTime << "\n";
    ss << "Avg  Time: " << (_totalResultTime / _calls) << "\n";
    ss << "Max  Time: " << _maxResultTime;

    _resultString = ss.str();
    return _resultString;
}