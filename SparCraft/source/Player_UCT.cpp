#include "Player_UCT.h"

using namespace SparCraft;

Player_UCT::Player_UCT (const size_t & playerID, const UCTSearchParameters & params) 
{
	_playerID = playerID;
    _params = params;
}

void Player_UCT::getMove(const GameState & state, Move & move)
{
    move.clear();
    
    UCTSearch uct(_params);

    uct.doSearch(state, move);
    _prevResults = uct.getResults();
}

UCTSearchParameters & Player_UCT::getParams()
{
    return _params;
}

UCTSearchResults & Player_UCT::getResults()
{
    return _prevResults;
}

PlayerPtr Player_UCT::clone()
{
    return PlayerPtr(new Player_UCT(*this));
}