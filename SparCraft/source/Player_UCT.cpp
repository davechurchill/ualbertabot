#include "Player_UCT.h"

using namespace SparCraft;

Player_UCT::Player_UCT (const IDType & playerID, const UCTSearchParameters & params) 
{
	_playerID = playerID;
    _params = params;
}

void Player_UCT::getMoves(GameState & state, const MoveArray & moves, std::vector<UnitAction> & moveVec)
{
    moveVec.clear();
    
    UCTSearch uct(_params);

    uct.doSearch(state, moveVec);
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