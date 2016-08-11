#include "Eval.h"
#include "Game.h"

namespace SparCraft
{
namespace Eval
{
    IDType PerformPlayout(const GameState & state, const PlayerPtr & p1, const PlayerPtr & p2)
    {
        Game g(state, p1, p2);
        g.play();
        return 0;//g.getState().winner();
    }

    ScoreType LTD2(const GameState & state, const IDType & player)
    {
	    if (state.numUnits(player) == 0)
	    {
		    return 0;
	    }

	    float sum(0);

	    for (IDType u(0); u<state.numUnits(player); ++u)
	    {
		    const Unit & unit = state.getUnit(player, u);

		    sum += sqrtf(unit.currentHP()) * unit.dpf();
	    }

	    ScoreType ret = 0;//(ScoreType)(1000 * sum / _totalSumSQRT[player]);

	    return ret;
    }

    ScoreType LTD(const GameState & state, const IDType & player)
    {
	    if (state.numUnits(player) == 0)
	    {
		    return 0;
	    }

	    float sum(0);

	    for (IDType u(0); u<state.numUnits(player); ++u)
	    {
		    const Unit & unit = state.getUnit(player, u);

		    sum += unit.currentHP() * unit.dpf();
	    }

	    return 0;//(ScoreType)(1000 * sum / _totalLTD[player]);
    }
}
}

