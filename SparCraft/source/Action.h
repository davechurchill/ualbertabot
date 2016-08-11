#pragma once

#include "Position.hpp"

namespace SparCraft
{

namespace ActionTypes
{
	enum {NONE, ATTACK, RELOAD, MOVE, PASS, HEAL};
};

class Action 
{
	UnitID      _unitID;
	PlayerID    _playerID;
	size_t      _actionType;
	UnitID      _targetID;

    Position _p;

public:


	Action();
    Action(const UnitID & unitIndex, const PlayerID & player, const size_t & type, const UnitID & moveIndex, const Position & dest);
	Action(const UnitID & unitIndex, const PlayerID & player, const size_t & type, const UnitID & moveIndex);

	const bool operator == (const Action & rhs);

	const UnitID & getID() const;
	const PlayerID & getPlayerID() const;
	const size_t & type() const;
	const size_t & getTargetID() const;
    const Position & pos() const;

	const std::string moveString() const;

	const Position getDir() const;

    const std::string debugString() const;

};

}