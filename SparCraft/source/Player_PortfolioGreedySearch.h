#pragma once

#include "Common.h"
#include "Player.h"
#include "PortfolioGreedySearch.h"

namespace SparCraft
{
class Player_PortfolioGreedySearch : public Player
{
	PlayerID _seed;
	size_t _iterations;
    size_t _responses;
    size_t _timeLimit;
public:
	Player_PortfolioGreedySearch (const PlayerID & playerID);
    Player_PortfolioGreedySearch (const PlayerID & playerID, const PlayerID & seed, const size_t & iter, const size_t & responses, const size_t & timeLimit);
	void getMoves(const GameState & state, std::vector<Action> & moveVec);
    virtual PlayerPtr clone();
};
}