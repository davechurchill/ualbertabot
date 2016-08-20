#pragma once

#include "Common.h"
#include "Player.h"
#include "PortfolioGreedySearch.h"
#include "PGSParameters.h"

namespace SparCraft
{
class Player_PortfolioGreedySearch : public Player
{
	PGSParameters _params;

public:

    Player_PortfolioGreedySearch (const size_t & playerID, const PGSParameters & params);
	void getMove(const GameState & state, Move & move);
    virtual PlayerPtr clone();
};
}