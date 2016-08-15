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
{

}

Game::Game(const GameState & initialState, PlayerPtr p1, PlayerPtr p2, const size_t turnLimit)
    : _numPlayers(0)
    , _state(initialState)
    , _playerToMoveMethod(SparCraft::PlayerToMove::Alternate)
    , _rounds(0)
    , _moveLimit(turnLimit)
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

    //_scriptMoves[0].clear();
    //_scriptMoves[1].clear();

    // the player that will move next
    const size_t playerToMove   = getPlayerToMove();
    PlayerPtr player            = _players[playerToMove];
    PlayerPtr enemy             = _players[_state.getEnemy(playerToMove)];

    // generate the moves possible from this state
    //_state.generateMoves(_moves[toMove->ID()], toMove->ID());

    const size_t whoCanMove = _state.whoCanMove();
    Move playerMove;
    Move enemyMove;

    int times[2] = {_state.getTimeNextUnitCanAct(0), _state.getTimeNextUnitCanAct(1) };

    // the tuple of moves he wishes to make
    player->getMove(_state, playerMove);
        
    // if both players can move, generate the other player's moves
    if (_state.bothCanMove())
    {
        enemy->getMove(_state, enemyMove);
    }

    int times2[2] = {_state.getTimeNextUnitCanAct(0), _state.getTimeNextUnitCanAct(1) };

    // make the moves
    _state.doMove(playerMove, enemyMove);

    //_state.finishedMoving();
    _rounds++;
}

// play the game until there is a winner
void Game::playIndividualScripts(UnitScriptData & scriptData)
{
    // array which will hold all the script moves for players
    Array2D<Move, Constants::Num_Players, PlayerModels::Size> allScriptMoves;

    _t.start();

    // play until there is no winner
    while (!gameOver())
    {
        if (_moveLimit && _rounds > _moveLimit)
        {
            break;
        }

        Timer frameTimer;
        frameTimer.start();

        // clear all script moves for both players
        for (size_t p(0); p<Constants::Num_Players; p++)
        {
            for (size_t s(0); s<PlayerModels::Size; ++s)
            {
                allScriptMoves[p][s].clear();
            }
        }

        // the playr that will move next
        const size_t playerToMove = getPlayerToMove();
        const size_t enemyPlayer  = _state.getEnemy(playerToMove);

        MoveArray playerLegalMoves;
        Move playerMove;

        // generate the moves possible from this state
        ActionGenerators::GenerateCompassActions(_state, playerToMove, playerLegalMoves);

        // calculate the moves the unit would do given its script preferences
        scriptData.calculateMoves(playerToMove, playerLegalMoves, _state, playerMove);

        // if both players can move, generate the other player's moves
        if (_state.bothCanMove())
        {
            MoveArray enemyLegalMoves;
            Move enemyMove;
            ActionGenerators::GenerateCompassActions(_state, enemyPlayer, enemyLegalMoves);

            scriptData.calculateMoves(enemyPlayer, enemyLegalMoves, _state, enemyMove);

            _state.doMove(enemyMove);
        }

        // make the moves
        _state.doMove(playerMove);
        _rounds++;
    }

    _gameTimeMS = _t.getElapsedTimeInMilliSec();
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
    return _state.isTerminal(); 
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
