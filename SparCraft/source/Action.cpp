#include "Common.h"
#include "Action.h"

using namespace SparCraft;

Action::Action()
	: _unitID(0)
	, _playerID(0)
	, _actionType(ActionTypes::NONE)
	, _targetID(0)
{

}

Action::Action(const size_t & unitIndex, const size_t & player, const size_t & type, const size_t & targetID, const Position & dest)
    : _unitID(unitIndex)
    , _playerID(player)
    , _actionType(type)
    , _targetID(targetID)
    , _p(dest)
{
        
}

Action::Action(const size_t & unitIndex, const size_t & player, const size_t & type, const size_t & targetID)
	: _unitID(unitIndex)
	, _playerID(player)
	, _actionType(type)
	, _targetID(targetID)
{
		
}

bool Action::operator == (const Action & rhs) const
{
	return _unitID == rhs._unitID && _playerID == rhs._playerID && _actionType == rhs._actionType && _targetID == rhs._targetID && _p == rhs._p;
}

bool Action::operator != (const Action & rhs) const
{
	return !(*this == rhs);
}

const size_t & Action::getID() const	
{ 
    return _unitID; 
}

const size_t & Action::getPlayerID() const	
{ 
    return _playerID; 
}

const size_t & Action::type() const	
{ 
    return _actionType; 
}

const size_t & Action::getTargetID() const	
{ 
    return _targetID; 
}

const Position & Action::pos() const   
{ 
    return _p; 
}

const std::string Action::moveString() const
{
	if (_actionType == ActionTypes::ATTACK) 
	{
		return "ATTACK";
	}
	else if (_actionType == ActionTypes::MOVE)
	{
		return "MOVE";
	}
	else if (_actionType == ActionTypes::RELOAD)
	{
		return "RELOAD";
	}
	else if (_actionType == ActionTypes::PASS)
	{
		return "PASS";
	}
	else if (_actionType == ActionTypes::HEAL)
	{
		return "HEAL";
	}

	return "NONE";
}

const Position Action::getDir() const
{
	return Position(Constants::Move_Dir[_targetID][0], Constants::Move_Dir[_targetID][1]);
}

const std::string Action::debugString() const
{
    std::stringstream ss;
    ss << moveString() << ": (" << (int)getID() << "," << (int)getPlayerID() << "," << (int)type() << "," << (int)getTargetID() << ")  " << "(" << pos().x() << "," << pos().y()   << ")";
    return ss.str();
}

