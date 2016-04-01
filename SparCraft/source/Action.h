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
	IDType  _unit;
	IDType	_player;
	IDType	_moveType;
	IDType	_moveIndex;

    Position _p;

public:


	Action();

    Action( const IDType & unitIndex, const IDType & player, const IDType & type, const IDType & moveIndex, const Position & dest);

	Action( const IDType & unitIndex, const IDType & player, const IDType & type, const IDType & moveIndex);

	const bool operator == (const Action & rhs);

	const IDType & unit()	const;
	const IDType & player() const;
	const IDType & type()	const;
	const IDType & index()	const;
    const Position & pos()  const;

	const std::string moveString() const;

	const Position getDir() const;

    const std::string debugString() const;

};

}