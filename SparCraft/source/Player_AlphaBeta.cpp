#include "Player_AlphaBeta.h"

using namespace SparCraft;

Player_AlphaBeta::Player_AlphaBeta (const IDType & playerID) 
{
	_playerID = playerID;
}

Player_AlphaBeta::Player_AlphaBeta (const IDType & playerID, const AlphaBetaSearchParameters & params, TTPtr table)
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

void Player_AlphaBeta::getMoves(GameState & state, const MoveArray & moves, std::vector<UnitAction> & moveVec)
{
    moveVec.clear();
	alphaBeta->doSearch(state);
    moveVec.assign(alphaBeta->getResults().bestMoves.begin(), alphaBeta->getResults().bestMoves.end());
}
