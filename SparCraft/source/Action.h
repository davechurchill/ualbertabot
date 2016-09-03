#pragma once

#include "Common.h"
#include "Position.hpp"

namespace SparCraft
{

namespace ActionTypes
{
    enum { NONE, ATTACK, MOVE, RELOAD, PASS, HEAL };
};

class Action 
{
	size_t          _playerID;
	size_t          _unitID;
    BWAPI::Order    _order;
	size_t          _targetID;
    Position        _position;

public:


	Action();
    Action(const size_t & playerID, const size_t & unitID, const BWAPI::Order & order, const size_t targetID = 0);
    Action(const size_t & playerID, const size_t & unitID, const BWAPI::Order & order, const Position & pos);

	bool operator == (const Action & rhs) const;
    bool operator != (const Action & rhs) const;

	const size_t &          getID() const;
	const size_t &          getPlayerID() const;
	const BWAPI::Order &    getOrder() const;
	const size_t &          getTargetID() const;
    const Position &        getPosition() const;
};

}