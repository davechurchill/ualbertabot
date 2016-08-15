#include "Player_PortfolioGreedySearch.h"

using namespace SparCraft;

Player_PortfolioGreedySearch::Player_PortfolioGreedySearch (const size_t & playerID) 
{
	_playerID = playerID;
	_iterations = 1;
    _responses = 0;
	_seed = PlayerModels::NOKDPS;
}

Player_PortfolioGreedySearch::Player_PortfolioGreedySearch (const size_t & playerID, const size_t & seed, const size_t & iter, const size_t & responses, const size_t & timeLimit)
{
	_playerID = playerID;
	_iterations = iter;
    _responses = responses;
	_seed = seed;
    _timeLimit = timeLimit;
}

void Player_PortfolioGreedySearch::getMove(const GameState & state, Move & move)
{
    move.clear();
	PortfolioGreedySearch pgs(_playerID, _seed, _iterations, _responses, _timeLimit);

	move = pgs.search(_playerID, state);
}

PlayerPtr Player_PortfolioGreedySearch::clone()
{
    return PlayerPtr(new Player_PortfolioGreedySearch(*this));
}