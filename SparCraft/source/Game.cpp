#include "Common.h"
#include "Game.h"

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
    const PlayerID playerToMove   = getPlayerToMove();
    PlayerPtr player            = _players[playerToMove];
    PlayerPtr enemy             = _players[_state.getEnemy(playerToMove)];

    // generate the moves possible from this state
    //_state.generateMoves(_moves[toMove->ID()], toMove->ID());

    std::vector<Action> playerMove;

    // the tuple of moves he wishes to make
    player->getMoves(_state, playerMove);
        
    // if both players can move, generate the other player's moves
    if (_state.bothCanMove())
    {
        std::vector<Action> enemyMove;
        enemy->getMoves(_state, enemyMove);

        _state.makeMoves(enemyMove);
    }

    // make the moves
    _state.makeMoves(playerMove);

    _state.finishedMoving();
    _rounds++;
}

// play the game until there is a winner
void Game::playIndividualScripts(UnitScriptData & scriptData)
{
    // array which will hold all the script moves for players
    Array2D<std::vector<Action>, Constants::Num_Players, PlayerModels::Size> allScriptMoves;

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
        for (PlayerID p(0); p<Constants::Num_Players; p++)
        {
            for (PlayerID s(0); s<PlayerModels::Size; ++s)
            {
                allScriptMoves[p][s].clear();
            }
        }

        // the playr that will move next
        const PlayerID playerToMove = getPlayerToMove();
        const PlayerID enemyPlayer  = _state.getEnemy(playerToMove);

        MoveArray playerLegalMoves;
        std::vector<Action> playerMove;

        // generate the moves possible from this state
        _state.generateMoves(playerLegalMoves, playerToMove);

        // calculate the moves the unit would do given its script preferences
        scriptData.calculateMoves(playerToMove, playerLegalMoves, _state, playerMove);

        // if both players can move, generate the other player's moves
        if (_state.bothCanMove())
        {
            MoveArray enemyLegalMoves;
            std::vector<Action> enemyMove;
            _state.generateMoves(enemyLegalMoves, enemyPlayer);

            scriptData.calculateMoves(enemyPlayer, enemyLegalMoves, _state, enemyMove);

            _state.makeMoves(enemyMove);
        }

        // make the moves
        _state.makeMoves(playerMove);
        _state.finishedMoving();
        _rounds++;
    }

    _gameTimeMS = _t.getElapsedTimeInMilliSec();
}

PlayerPtr Game::getPlayer(const PlayerID & player)
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
const PlayerID Game::getPlayerToMove()
{
    const PlayerID whoCanMove(_state.whoCanMove());

    return (whoCanMove == Players::Player_Both) ? Players::Player_One : whoCanMove;
}
