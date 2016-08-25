#include "PortfolioGreedySearch.h"
#include "Eval.h"

using namespace SparCraft;

PortfolioGreedySearch::PortfolioGreedySearch(const PGSParameters & params)
	: _params(params)
{
	
}

void PortfolioGreedySearch::calculatePortfolioScriptMoves(const GameState & state)
{
    for (size_t p(0); p < 2; ++p)
    {
        for (size_t i(0); i < _params.getPortfolio(p).size(); ++i)
        {
            PlayerPtr player = _params.getPortfolio(p)[i]->clone();

            Move m;
            player->getMove(state, m);
            _portfolioScriptMoves[p].push_back(m);
        }

        // set the units that can act based on the moves we just generated
        for (size_t i(0); i < _portfolioScriptMoves[p].back().size(); ++i)
        {
            const Action & action = _portfolioScriptMoves[p].back()[i];
            _activeUnitIDs[p].push_back(action.getID());
        }
    }

}

Move PortfolioGreedySearch::search(const GameState & state, const size_t & playerID)
{
    _searchTimer.start();

    const size_t enemyID = state.getEnemy(playerID);

    // pre-compute the actions done for each unit that can move for each Player in the portfolio
    calculatePortfolioScriptMoves(state);

    // calculate the seed scripts for each player
    // they will be used to seed the initial root search
    _seedScriptIndex[playerID] = calculateInitialSeed(state, playerID, _params.getEnemySeedPlayer());
    _seedScriptIndex[enemyID]  = calculateInitialSeed(state, enemyID,  _params.getPortfolio(playerID)[_seedScriptIndex[playerID]]);
    
    // set each of the current script assignments to the seed script
    for (size_t p(0); p < 2; ++p)
    {
        for (size_t u(0); u < state.numUnits(p); ++u)
        {
            _currentScriptAssignment[state.getUnit(p, u).getID()] = _seedScriptIndex[p];
        }
    }
    
    // run the base portfolio search
    doPortfolioSearch(state, playerID);

    // iterate as many times as required
    for (size_t r(0); r < _params.getResponses(); ++r)
    {
        // do the portfolio search to improve the enemy's scripts
        doPortfolioSearch(state, enemyID);

        // then do portfolio search again for us to improve vs. enemy's update
        doPortfolioSearch(state, playerID);
    }

    // construct the return move from the script assignments
    Move move;

    // we want to populate the move with one action per active unit for this player
    for (size_t u(0); u < _activeUnitIDs[playerID].size(); ++u)
    {
        // the unit ID is stored in the active unit ID vector
        const size_t unitID = _activeUnitIDs[playerID][u];

        // get the index of the script chosen for this unit from the script assignments
        const size_t scriptIndex = _currentScriptAssignment[unitID];

        // grab the action from the pre-computed moves from the portfolio
        const Action & scriptAction = _portfolioScriptMoves[playerID][scriptIndex][u];

        // add it to the move
        move.addAction(scriptAction);
    }
    
    return move;
}

void PortfolioGreedySearch::doPortfolioSearch(const GameState & state, const size_t & playerID)
{
    // the enemy of this player
    const size_t enemyID = state.getEnemy(playerID);

    for (size_t i(0); i < _params.getIterations(); ++i)
    {
        // for each unit that can move
        for (size_t unitIndex(0); unitIndex < _activeUnitIDs[playerID].size(); ++unitIndex)
        {
            if (_params.getTimeLimit() > 0 && _searchTimer.getElapsedTimeInMilliSec() > _params.getTimeLimit())
            {
                return;
            }

            const Unit & unit = state.getUnitByID(_activeUnitIDs[playerID][unitIndex]);
            size_t bestScriptIndex = 0;
            StateEvalScore bestScriptScore;

            // iterate over each script move that it can execute
            for (size_t sIndex(0); sIndex < _params.getPortfolio(playerID).size(); ++sIndex)
            {
                // set the current script for this unit
                _currentScriptAssignment[unit.getID()] = sIndex;

                // evaluate the current state given a playout with these unit scripts
                StateEvalScore score = eval(state, playerID);

                // if we have a better score, set it
                if (sIndex == 0 || score > bestScriptScore)
                {
                    bestScriptIndex = sIndex;
                    bestScriptScore = score;
                }
            }

            // set the current vector to the best move for use in future simulations
            _currentScriptAssignment[unit.getID()] = bestScriptIndex;
        }
    }
}

StateEvalScore PortfolioGreedySearch::eval(const GameState & state, const size_t & playerID)
{
    // Do a playout of the current state using the current script assignments
    GameState currentState(state);
    const size_t maxTurns = _params.getMaxPlayoutTurns();

    for (size_t turns(0); (!maxTurns || turns < maxTurns) && !currentState.gameOver(); ++turns)
    {
        const size_t whoCanMove = currentState.whoCanMove();
        const size_t playerToMove = (whoCanMove == Players::Player_Both) ? Players::Player_One : whoCanMove;

        Move turnMove[2];

        // compute all the portfolio player moves for this state
        std::vector<Move> portfolioMoves[2];
        for (size_t p(0); p < 2; ++p)
        {
            for (size_t i(0); i < _params.getPortfolio(p).size(); ++i)
            {
                PlayerPtr player = _params.getPortfolio(p)[i]->clone();

                Move m;
                player->getMove(currentState, m);
                portfolioMoves[p].push_back(m);
            }

            // add the actions to the player's move based on the script assignment
            for (size_t a(0); a < portfolioMoves[p].back().size(); ++a)
            {
                const size_t unitID = portfolioMoves[p].back()[a].getID();
                turnMove[p].addAction(portfolioMoves[p][_currentScriptAssignment[unitID]][a]);
            }
        }

        currentState.doMove(turnMove[0], turnMove[1]);
    }

    return Eval::Eval(currentState, playerID, EvaluationMethods::LTD2);
}

size_t PortfolioGreedySearch::calculateInitialSeed(const GameState & state, const size_t & playerID, const PlayerPtr & enemyPlayer)
{
    size_t bestScriptIndex = 0;
    StateEvalScore bestScriptScore;
    const size_t enemyPlayerID = state.getEnemy(playerID);
    
    PlayerPtr players[2];
    players[enemyPlayerID] = enemyPlayer;

    // evaluate every Player in this player's portfolio against the chosen enemy Player
    for (size_t s(0); s < _params.getPortfolio(playerID).size(); ++s)
    {
        PlayerPtr selfPlayer = _params.getPortfolio(playerID)[s]->clone();
        players[playerID] = selfPlayer;

        StateEvalScore score = Eval::Eval(state, playerID, EvaluationMethods::Playout, players[0], players[1]);

        if (s == 0 || score > bestScriptScore)
        {
            bestScriptScore = score;
            bestScriptIndex = s;
        }
    }

    return bestScriptIndex;
}
