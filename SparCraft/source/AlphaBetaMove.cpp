#include "AlphaBetaMove.h"

using namespace SparCraft;

AlphaBetaMove::AlphaBetaMove()
    : _isValid(false)
{
}

AlphaBetaMove::AlphaBetaMove(const std::vector<Action> & move,const bool & isValid)
    : _move(move)
    ,_isValid(isValid)
{
}

const bool AlphaBetaMove::isValid() const 
{ 
    return _isValid; 
}

const std::vector<Action> & AlphaBetaMove::moveVec() const 
{ 
    return _move; 
}

TTBestMove::TTBestMove()
{
}

TTBestMove::TTBestMove(const AlphaBetaMove & first,const AlphaBetaMove & second)
    : _firstMove(first)
    ,_secondMove(second)
{
}

const AlphaBetaMove & TTBestMove::firstMove() const 
{ 
    return _firstMove; 
}

const AlphaBetaMove & TTBestMove::secondMove() const 
{ 
    return _secondMove; 
}

AlphaBetaValue::AlphaBetaValue()
{
}

AlphaBetaValue::AlphaBetaValue(const StateEvalScore & score,const AlphaBetaMove & abMove)
    : _score(score)
    ,_move(abMove)
{
}

const StateEvalScore & AlphaBetaValue::score() const 
{ 
    return _score; 
}

const AlphaBetaMove & AlphaBetaValue::abMove() const 
{ 
    return _move; 
}