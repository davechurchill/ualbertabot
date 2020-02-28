#pragma once

#include "Common.h"
#include "Position.hpp"

namespace SparCraft
{

namespace ActionTypes
{
	enum {NONE, ATTACK, RELOAD, MOVE, PASS, HEAL};
};

class Action 
{
	size_t  _unit;
	size_t	_player;
	size_t	_moveType;
	size_t	_moveIndex;

    Position _p;

public:


	Action();

    Action( const size_t & unitIndex, const size_t & player, const size_t & type, const size_t & moveIndex, const Position & dest);

	Action( const size_t & unitIndex, const size_t & player, const size_t & type, const size_t & moveIndex);

	const bool operator == (const Action & rhs);

	const size_t & unit()	const;
	const size_t & player() const;
	const size_t & type()	const;
	const size_t & index()	const;
    const Position & pos()  const;

	const std::string moveString() const;

	const Position getDir() const;

    const std::string debugString() const;

};

}