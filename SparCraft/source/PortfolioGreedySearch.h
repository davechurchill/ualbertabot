#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "Game.h"
#include "Action.h"
#include "UnitScriptData.h"
#include "PGSParameters.h"
#include <memory>

namespace SparCraft
{

class PortfolioGreedySearch
{
    PGSParameters               _params;
    PlayerPtr                   _enemySeedPlayer;
    std::vector<PlayerPtr>      _playerPortfolio[2];
    std::vector<size_t>			_playerScriptPortfolio;
    size_t                      _totalEvals;
    size_t                      _timeLimit;

    void                        doPortfolioSearch(const size_t & player, const GameState & state, UnitScriptData & currentData);
    Move                        getmove(const size_t & player, const GameState & state, const std::vector<size_t> & playerScripts);
    StateEvalScore              eval(const size_t & player, const GameState & state, UnitScriptData & playerScriptsChosen);
    size_t                      calculateInitialSeed(const size_t & player, const GameState & state);
    void                        setAllScripts(const size_t & player, const GameState & state, UnitScriptData & data, const size_t & script);

public:

    PortfolioGreedySearch(const PGSParameters & params);

    Move searchOld(const size_t & player, const GameState & state);
    Move search(const size_t & player, const GameState & state);
};

}