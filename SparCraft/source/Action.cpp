#include "Common.h"
#include "Action.h"

using namespace SparCraft;

Action::Action()
	: _playerID(0)
	, _unitID(0)
	, _order(BWAPI::Orders::None)
	, _targetID(0)
{

}

Action::Action(const size_t & playerID, const size_t & unitID, const BWAPI::Order & order, const size_t targetID)
    : _playerID (playerID)
    , _unitID   (unitID)
    , _order    (order)
    , _targetID (targetID)
{

}

Action::Action(const size_t & playerID, const size_t & unitID, const BWAPI::Order & order, const Position & pos)
    : _playerID (playerID)
    , _unitID   (unitID)
    , _order    (order)
    , _targetID (0)
    , _position (pos)
{

}

bool Action::operator == (const Action & rhs) const
{
	return _unitID == rhs._unitID && _playerID == rhs._playerID && _order == rhs._order && _targetID == rhs._targetID && _position == rhs._position;
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

const BWAPI::Order & Action::getOrder() const	
{ 
    return _order; 
}

const size_t & Action::getTargetID() const	
{ 
    return _targetID; 
}

const Position & Action::getPosition() const   
{ 
    return _position; 
}
