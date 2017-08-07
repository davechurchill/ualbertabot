#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"

namespace SparCraft
{

class StateEvalScore
{
    double _val;
    size_t _numMoves;

public:

    StateEvalScore();
    StateEvalScore(const double & val, const size_t & numMoves);

    bool operator < (const StateEvalScore & rhs) const;
    bool operator > (const StateEvalScore & rhs) const;
    bool operator <= (const StateEvalScore & rhs) const;
    bool operator >= (const StateEvalScore & rhs) const;
    bool operator == (const StateEvalScore & rhs) const;

    const double & val() const;
    const size_t & numMoves() const;
};

namespace Eval
{
    size_t          PerformPlayout(const GameState & state, const PlayerPtr & p1, const PlayerPtr & p2);

    StateEvalScore  Eval(const GameState & state, const size_t & player, const size_t & evalMethod, PlayerPtr p1 = PlayerPtr(), PlayerPtr p2 = PlayerPtr());
    StateEvalScore  EvalSim(const GameState & state,const size_t & player, const PlayerPtr & p1, const PlayerPtr & p2, size_t moveLimit);
    double          EvalLTD(const GameState & state, const size_t & player);
    double          EvalLTD2(const GameState & state, const size_t & player);
    double          LTD(const GameState & state, const size_t & player);
    double          LTD2(const GameState & state, const size_t & player);
    double          TotalLTD(const GameState & state, const size_t & player);
    double          TotalLTD2(const GameState & state, const size_t & player);
}
}
