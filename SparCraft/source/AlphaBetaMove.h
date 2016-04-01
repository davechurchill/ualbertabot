#pragma once

#include "Common.h"
#include "Position.hpp"
#include "Action.h"

namespace SparCraft
{

class AlphaBetaMove
{
	std::vector<Action> _move;
	bool _isValid;

public:

	AlphaBetaMove();

	AlphaBetaMove(const std::vector<Action> & move, const bool & isValid);

	const bool isValid() const;
	const std::vector<Action> & moveVec() const;
};

class TTBestMove
{
	AlphaBetaMove _firstMove;
	AlphaBetaMove _secondMove;

public:

	TTBestMove();

	TTBestMove(const AlphaBetaMove & first, const AlphaBetaMove & second);

	const AlphaBetaMove & firstMove() const;
	const AlphaBetaMove & secondMove() const;
};


class AlphaBetaValue
{	
	StateEvalScore	_score;
	AlphaBetaMove	_move;

public:

	AlphaBetaValue();

	AlphaBetaValue(const StateEvalScore & score, const AlphaBetaMove & abMove);

	const StateEvalScore & score() const;
	const AlphaBetaMove & abMove() const;
};
}