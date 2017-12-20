#include "Common.h"
#include "Game.h"
#include "ActionGenerators.h"

using namespace SparCraft;

Game::Game(const GameState & initialState, size_t turnLimit)
    : _numPlayers(0)
    , _state(initialState)
    , _playerToMoveMethod(SparCraft::PlayerToMove::Alternate)
    , _rounds(0)
    , _moveLimit(turnLimit)
    , _simulateEveryFrame(false)
{
    _numMoves[0] = 0;
    _numMoves[1] = 0;
    _numActions[0] = 0;
    _numActions[1] = 0;
}

Game::Game(const GameState & initialState, PlayerPtr p1, PlayerPtr p2, const size_t turnLimit)
    : _numPlayers(0)
    , _state(initialState)
    , _playerToMoveMethod(SparCraft::PlayerToMove::Alternate)
    , _rounds(0)
    , _moveLimit(turnLimit)
    , _simulateEveryFrame(false)
{
    // add the players
    _players[Players::Player_One] = p1;
    _players[Players::Player_Two] = p2;
}

// play the game until there is a winner
void Game::play()
{
    _t.start();

    // play until there is no winner
    while (!gameOver())
    {
        if (_moveLimit && _rounds >= _moveLimit)
        {
            break;
        }

        playNextTurn();
    }

    _gameTimeMS = _t.getElapsedTimeInMilliSec();
}

void Game::playNextTurn()
{
    Timer frameTimer;
    frameTimer.start();

    const size_t whoCanMove = _state.whoCanMove();
    if (whoCanMove == Players::Player_None)
    {
        _state.setTime(_state.getTime() + 1);
        return;
    }

    // the player that will move next
    size_t playerToMove   = getPlayerToMove();
	if (playerToMove > 1)
	{
		// TODO: This is workaround for games with more then one opponent.
		playerToMove = Players::Player_Two;
	}

    PlayerPtr player            = _players[playerToMove];
    PlayerPtr enemy             = _players[_state.getEnemy(playerToMove)];

    Move playerMove;
    Move enemyMove;

    player->getMove(_state, playerMove);
        
    // if both players can move, generate the other player's moves
    if (_state.whoCanMove() == Players::Player_Both)
    {
        enemy->getMove(_state, enemyMove);
    }

    _state.doMove(playerMove, enemyMove, !_simulateEveryFrame);

    _rounds++;
}

void Game::setSimulateEveryFrame(bool sim)
{
    _simulateEveryFrame = sim;
}

PlayerPtr Game::getPlayer(const size_t & player)
{
    return _players[player];
}

int Game::getRounds()
{
    return _rounds;
}

double Game::getTime()
{
    return _gameTimeMS;
}

// returns whether or not the game is over
bool Game::gameOver() const
{
    return _state.gameOver(); 
}

const GameState & Game::getState() const
{
    return _state;
}

GameState & Game::getState()
{
    return _state;
}

// determine the player to move
const size_t Game::getPlayerToMove()
{
    const size_t whoCanMove = _state.whoCanMove();

    return (whoCanMove == Players::Player_Both) ? Players::Player_One : whoCanMove;
}
