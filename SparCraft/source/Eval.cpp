#include "Eval.h"
#include "Game.h"

using namespace SparCraft;


size_t Eval::PerformPlayout(const GameState & state, const PlayerPtr & p1, const PlayerPtr & p2)
{
    Game g(state,p1,p2);
    g.play();
    return 0;//g.getState().winner();
}

StateEvalScore Eval::Eval(const GameState & state, const size_t & player, const size_t & evalMethod, PlayerPtr p1, PlayerPtr p2)
{
	StateEvalScore score;
	const size_t enemyPlayer = state.getEnemy(player);

	if (state.playerDead(enemyPlayer) && state.playerDead(player))
	{
		return StateEvalScore(0, 0);
	}

	StateEvalScore simEval;

	if (evalMethod == SparCraft::EvaluationMethods::LTD)
	{
		score = StateEvalScore(EvalLTD(state, player), 0);
	}
	else if (evalMethod == SparCraft::EvaluationMethods::LTD2)
	{
		score = StateEvalScore(EvalLTD2(state, player), 0);
	}
	else if (evalMethod == SparCraft::EvaluationMethods::Playout)
	{
		score = EvalSim(state, player, p1, p2, 200);
	}

	if (score.val() == 0)
	{
		return score;
	}

	double winBonus(0);

	if (state.playerDead(enemyPlayer) && !state.playerDead(player))
	{
		winBonus = 100000;
	}
	else if (state.playerDead(player) && !state.playerDead(enemyPlayer))
	{
		winBonus = -100000;
	}

	return StateEvalScore(score.val() + winBonus, score.numMoves());
}

double Eval::EvalLTD(const GameState & state, const size_t & player) 
{
    const size_t enemyPlayer = state.getEnemy(player);

    return LTD(state, player) - LTD(state, enemyPlayer);
}

double Eval::EvalLTD2(const GameState & state, const size_t & player) 
{
    const size_t enemyPlayer = state.getEnemy(player);

    return LTD2(state, player) - LTD2(state, enemyPlayer);
}

StateEvalScore Eval::EvalSim(const GameState & state, const size_t & player, const PlayerPtr & p1, const PlayerPtr & p2, size_t moveLimit)
{
    Game game(state,p1,p2,moveLimit);

    game.play();

    double evalReturn = EvalLTD2(game.getState(), player);

    return StateEvalScore(evalReturn,game.getState().getNumMovements(player));
}

double Eval::LTD2(const GameState & state, const size_t & player)
{
    double sum = 0;

    for (size_t u(0); u<state.numUnits(player); ++u)
    {
        const Unit & unit = state.getUnit(player,u);
        
        sum += sqrtf(unit.currentHP()) * unit.dpf();
    }

    return sum;
}

double Eval::TotalLTD2(const GameState & state, const size_t & player)
{
    double sum = 0;

    for (const Unit & unit : state.getAllUnits())
    {
        if (unit.getPlayerID() != player)
        {
            continue;
        }

        sum += sqrtf((float)unit.type().maxHitPoints() + (float)unit.type().maxShields()) * unit.dpf();
    }

    return sum;
}

double Eval::LTD(const GameState & state, const size_t & player)
{
    float sum(0);

    for (size_t u(0); u<state.numUnits(player); ++u)
    {
        const Unit & unit = state.getUnit(player,u);

        sum += unit.currentHP() * unit.dpf();
    }

    return sum;
}

double Eval::TotalLTD(const GameState & state, const size_t & player)
{
    double sum = 0;

    for (const Unit & unit : state.getAllUnits())
    {
        if (unit.getPlayerID() != player)
        {
            continue;
        }

        sum += (unit.type().maxHitPoints() + unit.type().maxShields()) * unit.dpf();
    }

    return sum;
}


StateEvalScore::StateEvalScore()
    : _val(0)
    ,_numMoves(0)
{
}

StateEvalScore::StateEvalScore(const double & val, const size_t & numMoves)
    : _val(val)
    ,_numMoves(numMoves)
{
}

bool StateEvalScore::operator < (const StateEvalScore & rhs) const
{
    if (_val < rhs._val)
    {
        return true;
    }
    else if (_val == rhs._val)
    {
        return _numMoves > rhs._numMoves;
    }
    else
    {
        return false;
    }
}

bool StateEvalScore::operator > (const StateEvalScore & rhs) const
{
    if (_val > rhs._val)
    {
        return true;
    }
    else if (_val == rhs._val)
    {
        return _numMoves < rhs._numMoves;
    }
    else
    {
        return false;
    }
}

bool StateEvalScore::operator <= (const StateEvalScore & rhs) const
{
    if (_val > rhs._val)
    {
        return true;
    }
    else if (_val == rhs._val)
    {
        return _numMoves >= rhs._numMoves;
    }
    else
    {
        return false;
    }
}

bool StateEvalScore::operator >= (const StateEvalScore & rhs) const
{
    if (_val > rhs._val)
    {
        return true;
    }
    else if (_val == rhs._val)
    {
        return _numMoves <= rhs._numMoves;
    }
    else
    {
        return false;
    }
}

bool StateEvalScore::operator == (const StateEvalScore & rhs) const
{
    return (_val == rhs._val) && (_numMoves == rhs._numMoves);
}

const double & StateEvalScore::val() const 
{ 
    return _val; 
}

const size_t & StateEvalScore::numMoves() const 
{ 
    return _numMoves; 
}
