#include "PortfolioGreedySearch.h"

using namespace SparCraft;

PortfolioGreedySearch::PortfolioGreedySearch(const size_t & player, const size_t & enemyScript, const size_t & iter, const size_t & responses, const size_t & timeLimit)
	: _player(player)
	, _enemyScript(enemyScript)
	, _iterations(iter)
    , _responses(responses)
    , _totalEvals(0)
    , _timeLimit(timeLimit)
{
	_playerScriptPortfolio.push_back(PlayerModels::NOKDPS);
	_playerScriptPortfolio.push_back(PlayerModels::KiterDPS);
}

std::vector<Action> PortfolioGreedySearch::search(const size_t & player, const GameState & state)
{
    Timer t;
    t.start();

    const size_t enemyPlayer(state.getEnemy(player));

    // calculate the seed scripts for each player
    // they will be used to seed the initial root search
    size_t seedScript = calculateInitialSeed(player, state);
    size_t enemySeedScript = calculateInitialSeed(enemyPlayer, state);

    // set up the root script data
    UnitScriptData originalScriptData;
    setAllScripts(player, state, originalScriptData, seedScript);
    setAllScripts(enemyPlayer, state, originalScriptData, enemySeedScript);

    double ms = t.getElapsedTimeInMilliSec();
    //printf("\nFirst Part %lf ms\n", ms);

    // do the initial root portfolio search for our player
    UnitScriptData currentScriptData(originalScriptData);
    doPortfolioSearch(player, state, currentScriptData);

    // iterate as many times as required
    for (size_t i(0); i<_responses; ++i)
    {
        // do the portfolio search to improve the enemy's scripts
        doPortfolioSearch(enemyPlayer, state, currentScriptData);

        // then do portfolio search again for us to improve vs. enemy's update
        doPortfolioSearch(player, state, currentScriptData);
    }

    // convert the script vector into a move vector and return it
	MoveArray moves;
	state.generateMoves(moves, player);
    std::vector<Action> moveVec;
    GameState copy(state);
    currentScriptData.calculateMoves(player, moves, copy, moveVec);

    _totalEvals = 0;

    return moveVec;
}

void PortfolioGreedySearch::doPortfolioSearch(const size_t & player, const GameState & state, UnitScriptData & currentScriptData)
{
    Timer t;
    t.start();

    // the enemy of this player
    const size_t enemyPlayer(state.getEnemy(player));
    
    for (size_t i(0); i<_iterations; ++i)
    {
        // set up data for best scripts
        size_t          bestScriptVec[Constants::Max_Units];
	    StateEvalScore  bestScoreVec[Constants::Max_Units];

        // for each unit that can move
        for (size_t unitIndex(0); unitIndex<state.numUnits(player); ++unitIndex)
        {
            if (_timeLimit > 0 && t.getElapsedTimeInMilliSec() > _timeLimit)
            {
                break;
            }

            const Unit & unit(state.getUnit(player, unitIndex));

            // iterate over each script move that it can execute
            for (size_t sIndex(0); sIndex<_playerScriptPortfolio.size(); ++sIndex)
            {
                // set the current script for this unit
                currentScriptData.setUnitScript(unit, _playerScriptPortfolio[sIndex]);

                // evaluate the current state given a playout with these unit scripts
                StateEvalScore score = eval(player, state, currentScriptData);

                // if we have a better score, set it
                if (sIndex == 0 || score > bestScoreVec[unitIndex])
                {
                    bestScriptVec[unitIndex] = _playerScriptPortfolio[sIndex];
                    bestScoreVec[unitIndex]  = score;
                }
            }

            // set the current vector to the best move for use in future simulations
            currentScriptData.setUnitScript(unit, bestScriptVec[unitIndex]);
        }
    }   
}

size_t PortfolioGreedySearch::calculateInitialSeed(const size_t & player, const GameState & state)
{
    size_t bestScript;
    StateEvalScore bestScriptScore;
    const size_t enemyPlayer(state.getEnemy(player));
    
    // try each script in the portfolio for each unit as an initial seed
    for (size_t sIndex(0); sIndex<_playerScriptPortfolio.size(); ++sIndex)
    {
        UnitScriptData currentScriptData;
    
        // set the player's chosen script initially to the seed choice
        for (size_t unitIndex(0); unitIndex < state.numUnits(player); ++unitIndex)
        {
            currentScriptData.setUnitScript(state.getUnit(player, unitIndex), _playerScriptPortfolio[sIndex]);
        }

        // set the enemy units script choice to NOKDPS
        for (size_t unitIndex(0); unitIndex < state.numUnits(enemyPlayer); ++unitIndex)
        {
            currentScriptData.setUnitScript(state.getUnit(enemyPlayer, unitIndex), _enemyScript);
        }

        // evaluate the current state given a playout with these unit scripts
        StateEvalScore score = eval(player, state, currentScriptData);

        if (sIndex == 0 || score > bestScriptScore)
        {
            bestScriptScore = score;
            bestScript = _playerScriptPortfolio[sIndex];
        }
    }

    return bestScript;
}

StateEvalScore PortfolioGreedySearch::eval(const size_t & player, const GameState & state, UnitScriptData & playerScriptsChosen)
{
    const size_t enemyPlayer(state.getEnemy(player));

	Game g(state, 100);

    g.playIndividualScripts(playerScriptsChosen);

    _totalEvals++;

	return g.getState().eval(player, SparCraft::EvaluationMethods::LTD2);
}

void  PortfolioGreedySearch::setAllScripts(const size_t & player, const GameState & state, UnitScriptData & data, const size_t & script)
{
    for (size_t unitIndex(0); unitIndex < state.numUnits(player); ++unitIndex)
    {
        data.setUnitScript(state.getUnit(player, unitIndex), script);
    }
}

/*
std::vector<Action> PortfolioGreedySparCraft::search(const size_t & player, const GameState & state)
{
    const size_t enemyPlayer(state.getEnemy(player));
	GameState initialState(state);
	MoveArray moves;
	state.generateMoves(moves, player);

    size_t seedScript = calculateInitialSeed(player, state);

    UnitScriptData currentScriptData;
    
    // set the player's chosen script initially to the seed choice
    for (size_t unitIndex(0); unitIndex < state.numUnits(player); ++unitIndex)
    {
        currentScriptData.setUnitScript(state.getUnit(player, unitIndex), seedScript);
    }

    // set the enemy units script choice to the seed as well
    for (size_t unitIndex(0); unitIndex < state.numUnits(enemyPlayer); ++unitIndex)
    {
        currentScriptData.setUnitScript(state.getUnit(enemyPlayer, unitIndex), _enemyScript);
    }
	
    std::vector<size_t> bestScriptVec(moves.numUnits(), seedScript);
	std::vector<StateEvalScore> bestScoreVec(moves.numUnits());

    // the current script vector we will be working with
    std::vector<size_t> currentScriptVec(moves.numUnits(), seedScript);

    // for each unit that can move
    for (size_t unitIndex(0); unitIndex<moves.numUnits(); ++unitIndex)
    {
        const Unit & unit(state.getUnit(player, unitIndex));

        // iterate over each script move that it can execute
        for (size_t sIndex(0); sIndex<_playerScriptPortfolio.size(); ++sIndex)
        {
            // set the current script for this unit
            currentScriptData.setUnitScript(unit, _playerScriptPortfolio[sIndex]);

            // evaluate the current state given a playout with these unit scripts
            StateEvalScore score = evalAfterMoves(player, state, currentScriptData);

            // if we have a better score, set it
            if (sIndex == 0 || score > bestScoreVec[unitIndex])
            {
                bestScriptVec[unitIndex] = _playerScriptPortfolio[sIndex];
                bestScoreVec[unitIndex]  = score;
            }
        }

        // set the current vector to the best move for use in future simulations
        currentScriptData.setUnitScript(unit, bestScriptVec[unitIndex]);
    }
	
    std::vector<Action> moveVec;
    currentScriptData.calculateMoves(player, moves, GameState(state), moveVec);
    return moveVec;
}*/