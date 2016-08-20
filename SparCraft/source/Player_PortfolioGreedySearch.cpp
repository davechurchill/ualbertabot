#include "Player_PortfolioGreedySearch.h"

using namespace SparCraft;

Player_PortfolioGreedySearch::Player_PortfolioGreedySearch (const size_t & playerID, const PGSParameters & params)
    : _params(params)
{
	_playerID = playerID;
}

void Player_PortfolioGreedySearch::getMove(const GameState & state, Move & move)
{
    move.clear();

	PortfolioGreedySearch pgs(_params);

	move = pgs.search(_playerID, state);
}

PlayerPtr Player_PortfolioGreedySearch::clone()
{
    return PlayerPtr(new Player_PortfolioGreedySearch(*this));
}