#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "AllPlayers.h"
#include "Action.h"
#include <memory>
#include <set>

namespace SparCraft
{
	
typedef	std::shared_ptr<Player> PlayerPtr;


class UnitScriptData
{
    // map from UnitID to PlayerModel
    std::map<int, size_t>   _unitScriptMap[2];
    std::set<size_t>        _scriptSet[2];
    std::vector<size_t>     _scriptVec[2];
    std::vector<PlayerPtr>  _playerPtrVec[2];
    
   
    std::vector<Action>       _allScriptMoves[2][PlayerModels::Size];

    std::vector<Action> & getMoves(const size_t & player, const size_t & actualScript);

    Action & getMove(const size_t & player, const size_t & unitIndex, const size_t & actualScript);

public:

    UnitScriptData();

    void calculateMoves(const size_t & player, MoveArray & moves, GameState & state, std::vector<Action> & moveVec);
    void setUnitScript(const size_t & player, const int & id, const size_t & script);
    void setUnitScript(const Unit & unit, const size_t & script);

    const size_t &      getUnitScript(const size_t & player, const int & id) const;
    const size_t &      getUnitScript(const Unit & unit) const;
    const size_t &      getScript(const size_t & player, const size_t & index);
    const PlayerPtr &   getPlayerPtr(const size_t & player, const size_t & index);
    const size_t        getNumScripts(const size_t & player) const;
};
}