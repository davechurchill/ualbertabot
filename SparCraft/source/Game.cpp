#include "Game.h"

using namespace SparCraft;

Game::Game(const GameState & initialState, const size_t & limit)
    : _numPlayers(0)
    , state(initialState)
    , _playerToMoveMethod(SparCraft::PlayerToMove::Alternate)
    , rounds(0)
    , moveLimit(limit)
{
#ifdef USING_VISUALIZATION_LIBRARIES
    disp = NULL;
#endif
}

Game::Game(const GameState & initialState, PlayerPtr & p1, PlayerPtr & p2, const size_t & limit)
    : _numPlayers(0)
    , state(initialState)
    , _playerToMoveMethod(SparCraft::PlayerToMove::Alternate)
    , rounds(0)
    , moveLimit(limit)
{
    // add the players
    _players[Players::Player_One] = p1;
    _players[Players::Player_Two] = p2;

#ifdef USING_VISUALIZATION_LIBRARIES
    disp = NULL;
#endif
}

#ifdef USING_VISUALIZATION_LIBRARIES
void Game::setDisplay(Display * d)
{
    disp = d;
}
#endif

// play the game until there is a winner
void Game::play()
{
    scriptMoves[Players::Player_One] = std::vector<UnitAction>(state.numUnits(Players::Player_One));
    scriptMoves[Players::Player_Two] = std::vector<UnitAction>(state.numUnits(Players::Player_Two));

    t.start();

    // play until there is no winner
    while (!gameOver())
    {
        if (moveLimit && rounds >= moveLimit)
        {
            break;
        }

        Timer frameTimer;
        frameTimer.start();

        scriptMoves[0].clear();
        scriptMoves[1].clear();

        // the player that will move next
        const IDType playerToMove(getPlayerToMove());
        PlayerPtr & toMove = _players[playerToMove];
        PlayerPtr & enemy = _players[state.getEnemy(playerToMove)];

        // generate the moves possible from this state
        state.generateMoves(moves[toMove->ID()], toMove->ID());

        // the tuple of moves he wishes to make
        toMove->getMoves(state, moves[playerToMove], scriptMoves[playerToMove]);
        
        // if both players can move, generate the other player's moves
        if (state.bothCanMove())
        {
            state.generateMoves(moves[enemy->ID()], enemy->ID());
            enemy->getMoves(state, moves[enemy->ID()], scriptMoves[enemy->ID()]);

            state.makeMoves(scriptMoves[enemy->ID()]);
        }

        // make the moves
        state.makeMoves(scriptMoves[toMove->ID()]);

#ifdef USING_VISUALIZATION_LIBRARIES
        if (disp)
        {
            GameState copy(state);
            GameState copy2(state);
            copy2.finishedMoving();

            TimeType nextTime = std::min(copy2.getUnit(0, 0).firstTimeFree(), copy2.getUnit(1, 0).firstTimeFree());

            // set the parameter overlays for search players
            for (IDType p(0); p<Constants::Num_Players; ++p)
            {
                Player_UCT *        uct = dynamic_cast<Player_UCT *>        (_players[p].get());
                Player_AlphaBeta *  ab  = dynamic_cast<Player_AlphaBeta *>  (_players[p].get());

                if (uct) 
                { 
                    disp->SetParams(p, uct->getParams().getDescription());
                    disp->SetResults(p, uct->getResults().getDescription());
                }

                if (ab) 
                { 
                    disp->SetParams(p, ab->getParams().getDescription()); 
                    disp->SetResults(p, ab->results().getDescription());
                }
            }
            
            bool everyFrame = true;
            if (everyFrame)
            {
                while (copy.getTime() < nextTime)
                {
                    while (frameTimer.getElapsedTimeInMilliSec() < 30);
                    frameTimer.start();
                    disp->SetState(copy);
                    disp->OnFrame();
                    copy.setTime(copy.getTime()+1);
                }
            }
            else
            { 
                disp->SetState(state);
                disp->OnFrame();
            }
        }
#endif

        state.finishedMoving();
        rounds++;
    }

    gameTimeMS = t.getElapsedTimeInMilliSec();
}

// play the game until there is a winner
void Game::playIndividualScripts(UnitScriptData & scriptData)
{
    // array which will hold all the script moves for players
    Array2D<std::vector<UnitAction>, Constants::Num_Players, PlayerModels::Size> allScriptMoves;

    scriptMoves[Players::Player_One] = std::vector<UnitAction>(state.numUnits(Players::Player_One));
    scriptMoves[Players::Player_Two] = std::vector<UnitAction>(state.numUnits(Players::Player_Two));

    t.start();

    // play until there is no winner
    while (!gameOver())
    {
        if (moveLimit && rounds > moveLimit)
        {
            break;
        }

        Timer frameTimer;
        frameTimer.start();

        // clear all script moves for both players
        for (IDType p(0); p<Constants::Num_Players; p++)
        {
            for (IDType s(0); s<PlayerModels::Size; ++s)
            {
                allScriptMoves[p][s].clear();
            }
        }

        // clear the moves we will actually be doing
        scriptMoves[0].clear();
        scriptMoves[1].clear();

        // the playr that will move next
        const IDType playerToMove(getPlayerToMove());
        const IDType enemyPlayer(state.getEnemy(playerToMove));

        // generate the moves possible from this state
        state.generateMoves(moves[playerToMove], playerToMove);

        // calculate the moves the unit would do given its script preferences
        scriptData.calculateMoves(playerToMove, moves[playerToMove], state, scriptMoves[playerToMove]);

        // if both players can move, generate the other player's moves
        if (state.bothCanMove())
        {
            state.generateMoves(moves[enemyPlayer], enemyPlayer);

            scriptData.calculateMoves(enemyPlayer, moves[enemyPlayer], state, scriptMoves[enemyPlayer]);

            state.makeMoves(scriptMoves[enemyPlayer]);
        }

        // make the moves
        state.makeMoves(scriptMoves[playerToMove]);
        state.finishedMoving();
        rounds++;
    }

    gameTimeMS = t.getElapsedTimeInMilliSec();
}

int Game::getRounds()
{
    return rounds;
}

double Game::getTime()
{
    return gameTimeMS;
}

// returns whether or not the game is over
bool Game::gameOver()
{
    return state.isTerminal(); 
}

GameState & Game::getState()
{
    return state;
}

// determine the player to move
const IDType Game::getPlayerToMove()
{
    const IDType whoCanMove(state.whoCanMove());

    return (whoCanMove == Players::Player_Both) ? Players::Player_One : whoCanMove;
}
