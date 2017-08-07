#include "TournamentGame.h"

using namespace SparCraft;

TournamentGame::TournamentGame(const GameState & initialState, const std::string & p1name, PlayerPtr p1, const std::string & p2name, const PlayerPtr p2)
{
    _currentState = initialState;
    _players[0] = p1;
    _players[1] = p2;
    _playerNames[0] = p1name;
    _playerNames[1] = p2name;
    _playerTotalTimeMS[0] = 0;
    _playerTotalTimeMS[1] = 0;
    _maxTimeMS[0] = 0;
    _maxTimeMS[1] = 0;
    _totalMoves[0] = 0;
    _totalMoves[1] = 0;
    _totalActions[0] = 0;
    _totalActions[1] = 0;
}

void TournamentGame::playGame()
{
    Timer t;
    while(!_currentState.gameOver())
    {
        const size_t whoCanMove = _currentState.whoCanMove();

        Move moves[2];
        for (size_t p(0); p < 2; ++p)
        {
            if (whoCanMove != Players::Player_Both && whoCanMove != p)
            {
                continue;
            }

            Timer t;
            t.start();

            _players[p]->getMove(_currentState, moves[p]);
         
            double ms = t.getElapsedTimeInMilliSec();
            _playerTotalTimeMS[p] += ms;
            _maxTimeMS[p] = std::max((size_t)ms, _maxTimeMS[p]);
            _totalMoves[p]++;
            _totalActions[p] += moves[p].size();
        }

        _currentState.doMove(moves[0], moves[1]);
    }
}

const std::string & TournamentGame::getPlayerName(const size_t & player) const
{
    return _playerNames[player];
}

const GameState & TournamentGame::getFinalGameState() const
{
    return _currentState;
}

const size_t TournamentGame::getTotalTimeMS(const size_t & player) const
{
    return _playerTotalTimeMS[player];
}

const size_t TournamentGame::getMaxTimeMS(const size_t & player) const
{
    return _maxTimeMS[player];
}

const size_t TournamentGame::getTotalMoves(const size_t & player) const
{
    return _totalMoves[player];
}