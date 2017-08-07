#include "Player_PortfolioGreedySearch.h"

using namespace SparCraft;

Player_PortfolioGreedySearch::Player_PortfolioGreedySearch (const size_t & playerID, const PGSParameters & params)
    : Player()
    , _params(params)
    , _computedDescription(false)
{
	_playerID = playerID;
}

void Player_PortfolioGreedySearch::getMove(const GameState & state, Move & move)
{
    startTimer();
    move.clear();

	PortfolioGreedySearch pgs(_params);

	move = pgs.search(state, _playerID);
    stopTimer();
}

PlayerPtr Player_PortfolioGreedySearch::clone()
{
    return PlayerPtr(new Player_PortfolioGreedySearch(*this));
}

const std::string & Player_PortfolioGreedySearch::getDescription()
{
    if (!_computedDescription)
    {
        std::stringstream ss;
        ss << _name;
        ss << "\n";
        ss << _params.toString();
        _description = ss.str();

        _computedDescription = true;
    }

    return _description;
}
