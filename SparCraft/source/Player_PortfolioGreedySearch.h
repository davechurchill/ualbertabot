#pragma once

#include "Common.h"
#include "Player.h"
#include "PortfolioGreedySearch.h"

namespace SparCraft
{
class Player_PortfolioGreedySearch : public Player
{
	IDType _seed;
	size_t _iterations;
    size_t _responses;
    size_t _timeLimit;
public:
	Player_PortfolioGreedySearch (const IDType & playerID);
    Player_PortfolioGreedySearch (const IDType & playerID, const IDType & seed, const size_t & iter, const size_t & responses, const size_t & timeLimit);
	void getMoves(GameState & state, const MoveArray & moves, std::vector<UnitAction> & moveVec);
    IDType getType() { return PlayerModels::PortfolioGreedySearch; }
};
}