#pragma once

#include "Common.h"
#include "Player.h"
#include "AllPlayers.h"
#include "UCTSearch.h"
#include "UCTMemoryPool.hpp"

namespace SparCraft
{
class Player_UCT : public Player
{
    UCTSearchParameters     _params;
    UCTSearchResults        _prevResults;
public:
    Player_UCT (const size_t & playerID, const UCTSearchParameters & params);
	void getMove(const GameState & state, Move & move);
    UCTSearchParameters & getParams();
    UCTSearchResults & getResults();
    virtual PlayerPtr clone();
};
}