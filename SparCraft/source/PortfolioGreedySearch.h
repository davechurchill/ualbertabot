#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "Game.h"
#include "Action.h"
#include "PGSParameters.h"
#include "Eval.h"
#include <memory>

namespace SparCraft
{

class PortfolioGreedySearch
{
    PGSParameters               _params;
    PlayerPtr                   _enemySeedPlayer;
    size_t                      _seedScriptIndex[2];
    std::vector<size_t>         _activeUnitIDs[2];
    Timer                       _searchTimer;

    std::vector<Move>           _portfolioScriptMoves[2];
    std::unordered_map<size_t, size_t> _currentScriptAssignment;
    
    void                        doPortfolioSearch(const GameState & state, const size_t & playerID);
    void                        calculatePortfolioScriptMoves(const GameState & state);
    size_t                      calculateInitialSeed(const GameState & state, const size_t & playerID, const PlayerPtr & enemyPlayer);
    StateEvalScore              eval(const GameState & state, const size_t & playerID);

public:

    PortfolioGreedySearch(const PGSParameters & params);

    Move search(const GameState & state, const size_t & player);
};

}