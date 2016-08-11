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
	UnitID      _unit;
	PlayerID    _player;
	size_t      _moveType;
	size_t      _moveIndex;

    Position _p;

public:


	Action();

    Action( const PlayerID & unitIndex, const PlayerID & player, const PlayerID & type, const PlayerID & moveIndex, const Position & dest);

	Action( const PlayerID & unitIndex, const PlayerID & player, const PlayerID & type, const PlayerID & moveIndex);

	const bool operator == (const Action & rhs);

	const UnitID & getUnitID() const;
	const PlayerID & getPlayerID() const;
	const size_t & type()	const;
	const size_t & index()	const;
    const Position & pos()  const;

	const std::string moveString() const;

	const Position getDir() const;

    const std::string debugString() const;

};

}