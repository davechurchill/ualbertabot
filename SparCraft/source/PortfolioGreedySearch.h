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
	
    const size_t				_player;
    const size_t				_enemyScript;
    const size_t				_iterations;
    const size_t                _responses;
    std::vector<size_t>			_playerScriptPortfolio;
    size_t                      _totalEvals;
    size_t                      _timeLimit;

    void                        doPortfolioSearch(const size_t & player,const GameState & state,UnitScriptData & currentData);
    std::vector<Action>     getMoveVec(const size_t & player,const GameState & state,const std::vector<size_t> & playerScripts);
    StateEvalScore              eval(const size_t & player,const GameState & state,UnitScriptData & playerScriptsChosen);
    size_t                      calculateInitialSeed(const size_t & player,const GameState & state);
    void                        setAllScripts(const size_t & player,const GameState & state,UnitScriptData & data,const size_t & script);

public:

    PortfolioGreedySearch(const size_t & player, const size_t & enemyScript, const size_t & iter, const size_t & responses, const size_t & timeLimit);
    std::vector<Action> search(const size_t & player, const GameState & state);
};

}