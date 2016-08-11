#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "Game.h"
#include "Action.h"
#include "UnitScriptData.h"
#include <memory>

namespace SparCraft
{
	
typedef	std::shared_ptr<Player> PlayerPtr;

class PortfolioGreedySearch
{
protected:
	
    const PlayerID				_player;
    const PlayerID				_enemyScript;
    const size_t				_iterations;
    const size_t                _responses;
    std::vector<PlayerID>			_playerScriptPortfolio;
    size_t                      _totalEvals;
    size_t                      _timeLimit;

    void                        doPortfolioSearch(const PlayerID & player,const GameState & state,UnitScriptData & currentData);
    std::vector<Action>         getMoveVec(const PlayerID & player,const GameState & state,const std::vector<PlayerID> & playerScripts);
    StateEvalScore              eval(const PlayerID & player,const GameState & state,UnitScriptData & playerScriptsChosen);
    PlayerID                      calculateInitialSeed(const PlayerID & player,const GameState & state);
    void                        setAllScripts(const PlayerID & player,const GameState & state,UnitScriptData & data,const PlayerID & script);

public:

    PortfolioGreedySearch(const PlayerID & player, const PlayerID & enemyScript, const size_t & iter, const size_t & responses, const size_t & timeLimit);
    std::vector<Action> search(const PlayerID & player, const GameState & state);
};

}