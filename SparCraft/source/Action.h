#pragma once

#include "Position.hpp"

namespace SparCraft
{

namespace ActionTypes
{
    enum { NONE, ATTACK, MOVE, RELOAD, PASS };
};

class Action 
{
	size_t      _unitID;
	size_t      _playerID;
	size_t      _actionType;
	size_t      _targetID;

    Position _p;

public:


	Action();
    Action(const size_t & unitIndex, const size_t & player, const size_t & type, const size_t & moveIndex, const Position & dest);
	Action(const size_t & unitIndex, const size_t & player, const size_t & type, const size_t & moveIndex);

	bool operator == (const Action & rhs) const;
    bool operator != (const Action & rhs) const;

	const size_t & getID() const;
	const size_t & getPlayerID() const;
	const size_t & type() const;
	const size_t & getTargetID() const;
    const Position & pos() const;

	const std::string moveString() const;

	const Position getDir() const;

    const std::string debugString() const;

};

}