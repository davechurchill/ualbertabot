#include "Player_AlphaBeta.h"

using namespace SparCraft;

Player_AlphaBeta::Player_AlphaBeta (const size_t & playerID) 
{
	_playerID = playerID;
}

Player_AlphaBeta::Player_AlphaBeta (const size_t & playerID, const AlphaBetaSearchParameters & params, TTPtr table)
{
	_playerID = playerID;
	_params = params;
	TT = table;

    alphaBeta = new AlphaBetaSearch(_params, TT);
}

Player_AlphaBeta::~Player_AlphaBeta()
{
    delete alphaBeta;
}

AlphaBetaSearchResults & Player_AlphaBeta::results()
{
	return alphaBeta->getResults();
}

AlphaBetaSearchParameters & Player_AlphaBeta::getParams()
{
	return _params;
}

void Player_AlphaBeta::setParameters(AlphaBetaSearchParameters & p)
{
	_params = p;
}

void Player_AlphaBeta::setTranspositionTable(TTPtr table)
{
	TT = table;
}

void Player_AlphaBeta::getMove(const GameState & state, Move & move)
{
    move.clear();
	alphaBeta->doSearch(state);

    move = alphaBeta->getResults().bestMoves;
}

PlayerPtr Player_AlphaBeta::clone()
{
    return PlayerPtr(new Player_AlphaBeta(*this));
}