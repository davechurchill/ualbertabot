#include "Action.h"

using namespace SparCraft;

Action::Action()
	: _unit(255)
	, _player(255)
	, _moveType(ActionTypes::NONE)
	, _moveIndex(255)
{

}

Action::Action( const IDType & unitIndex, const IDType & player, const IDType & type, const IDType & moveIndex, const Position & dest)
    : _unit(unitIndex)
    , _player(player)
    , _moveType(type)
    , _moveIndex(moveIndex)
    , _p(dest)
{
        
}

Action::Action( const IDType & unitIndex, const IDType & player, const IDType & type, const IDType & moveIndex)
	: _unit(unitIndex)
	, _player(player)
	, _moveType(type)
	, _moveIndex(moveIndex)
{
		
}

const bool Action::operator == (const Action & rhs)
{
	return _unit == rhs._unit && _player == rhs._player && _moveType == rhs._moveType && _moveIndex == rhs._moveIndex && _p == rhs._p;
}

const IDType & Action::unit() const	
{ 
    return _unit; 
}

const IDType & Action::player() const	
{ 
    return _player; 
}

const IDType & Action::type() const	
{ 
    return _moveType; 
}

const IDType & Action::index() const	
{ 
    return _moveIndex; 
}

const Position & Action::pos() const   
{ 
    return _p; 
}

const std::string Action::moveString() const
{
	if (_moveType == ActionTypes::ATTACK) 
	{
		return "ATTACK";
	}
	else if (_moveType == ActionTypes::MOVE)
	{
		return "MOVE";
	}
	else if (_moveType == ActionTypes::RELOAD)
	{
		return "RELOAD";
	}
	else if (_moveType == ActionTypes::PASS)
	{
		return "PASS";
	}
	else if (_moveType == ActionTypes::HEAL)
	{
		return "HEAL";
	}

	return "NONE";
}

const Position Action::getDir() const
{
	return Position(Constants::Move_Dir[_moveIndex][0], Constants::Move_Dir[_moveIndex][1]);
}

const std::string Action::debugString() const
{
    std::stringstream ss;
    ss << moveString() << ": (" << (int)unit() << "," << (int)player() << "," << (int)type() << "," << (int)index() << ")  " << "(" << pos().x() << "," << pos().y()   << ")";
    return ss.str();
}

