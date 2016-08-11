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
    std::map<int, PlayerID>   _unitScriptMap[2];
    std::set<PlayerID>        _scriptSet[2];
    std::vector<PlayerID>     _scriptVec[2];
    std::vector<PlayerPtr>  _playerPtrVec[2];
    
   
    std::vector<Action>       _allScriptMoves[2][PlayerModels::Size];

    std::vector<Action> & getMoves(const PlayerID & player, const PlayerID & actualScript);

    Action & getMove(const PlayerID & player, const PlayerID & unitIndex, const PlayerID & actualScript);

public:

    UnitScriptData();

    void calculateMoves(const PlayerID & player, MoveArray & moves, GameState & state, std::vector<Action> & moveVec);
    void setUnitScript(const PlayerID & player, const int & id, const PlayerID & script);
    void setUnitScript(const Unit & unit, const PlayerID & script);

    const PlayerID &      getUnitScript(const PlayerID & player, const int & id) const;
    const PlayerID &      getUnitScript(const Unit & unit) const;
    const PlayerID &      getScript(const PlayerID & player, const size_t & index);
    const PlayerPtr &   getPlayerPtr(const PlayerID & player, const size_t & index);
    const size_t        getNumScripts(const PlayerID & player) const;
};
}