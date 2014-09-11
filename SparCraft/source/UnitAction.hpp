#pragma once

#include "Common.h"
#include "Position.hpp"

namespace SparCraft
{

namespace UnitActionTypes
{
	enum {NONE, ATTACK, RELOAD, MOVE, PASS, HEAL};
};

class UnitAction 
{

public:

	IDType			_unit,
					_player,
					_moveType,
					_moveIndex;

    Position        _p;

	UnitAction()
		: _unit(255)
		, _player(255)
		, _moveType(UnitActionTypes::NONE)
		, _moveIndex(255)
	{

	}

    UnitAction( const IDType & unitIndex, const IDType & player, const IDType & type, const IDType & moveIndex, const Position & dest)
        : _unit(unitIndex)
        , _player(player)
        , _moveType(type)
        , _moveIndex(moveIndex)
        , _p(dest)
    {
        
    }

	UnitAction( const IDType & unitIndex, const IDType & player, const IDType & type, const IDType & moveIndex)
		: _unit(unitIndex)
		, _player(player)
		, _moveType(type)
		, _moveIndex(moveIndex)
	{
		
	}

	const bool operator == (const UnitAction & rhs)
	{
		return _unit == rhs._unit && _player == rhs._player && _moveType == rhs._moveType && _moveIndex == rhs._moveIndex && _p == rhs._p;
	}

	const IDType & unit()	const	{ return _unit; }
	const IDType & player() const	{ return _player; }
	const IDType & type()	const	{ return _moveType; }
	const IDType & index()	const	{ return _moveIndex; }
    const Position & pos()  const   { return _p; }

	const std::string moveString() const
	{
		if (_moveType == UnitActionTypes::ATTACK) 
		{
			return "ATTACK";
		}
		else if (_moveType == UnitActionTypes::MOVE)
		{
			return "MOVE";
		}
		else if (_moveType == UnitActionTypes::RELOAD)
		{
			return "RELOAD";
		}
		else if (_moveType == UnitActionTypes::PASS)
		{
			return "PASS";
		}
		else if (_moveType == UnitActionTypes::HEAL)
		{
			return "HEAL";
		}

		return "NONE";
	}

	const Position getDir() const
	{
		return Position(Constants::Move_Dir[_moveIndex][0], Constants::Move_Dir[_moveIndex][1]);
	}

    const std::string debugString() const
    {
        std::stringstream ss;
        ss << moveString() << ": (" << (int)unit() << "," << (int)player() << "," << (int)type() << "," << (int)index() << ")  " << "(" << pos().x() << "," << pos().y()   << ")";
        return ss.str();
    }
};


class AlphaBetaMove
{
	std::vector<UnitAction> _move;
	bool _isValid;

public:

	AlphaBetaMove()
        : _isValid(false)
	{
	}

	AlphaBetaMove(const std::vector<UnitAction> & move, const bool & isValid)
		: _move(move)
		, _isValid(isValid)
	{
	}

	const bool isValid() const { return _isValid; }
	const std::vector<UnitAction> & moveVec() const { return _move; }
};

class TTBestMove
{
	AlphaBetaMove _firstMove;
	AlphaBetaMove _secondMove;

public:

	TTBestMove()
	{
	}

	TTBestMove(const AlphaBetaMove & first, const AlphaBetaMove & second)
		: _firstMove(first)
		, _secondMove(second)
	{
	}

	const AlphaBetaMove & firstMove() const		{ return _firstMove; }
	const AlphaBetaMove & secondMove() const	{ return _secondMove; }
};


class AlphaBetaValue
{	
	StateEvalScore	_score;
	AlphaBetaMove	_move;

public:

	AlphaBetaValue()
	{
	}

	AlphaBetaValue(const StateEvalScore & score, const AlphaBetaMove & abMove)
		: _score(score)
		, _move(abMove)
	{
	}

	const StateEvalScore & score() const { return _score; }
	const AlphaBetaMove & abMove() const { return _move; }
};
}