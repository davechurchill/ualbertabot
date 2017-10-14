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
    bool _computedDescription;

public:

    Player_PortfolioGreedySearch (const size_t & playerID, const PGSParameters & params);
	void getMove(const GameState & state, Move & move);
    virtual const std::string & getDescription();
    virtual PlayerPtr clone();
};
}