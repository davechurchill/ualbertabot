#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "AllPlayers.h"
#include "UnitAction.hpp"
#include <boost/shared_ptr.hpp>

namespace SparCraft
{
	
typedef	boost::shared_ptr<Player> PlayerPtr;


class UnitScriptData
{
    // map from UnitID to PlayerModel
    std::map<int, IDType>   _unitScriptMap[2];
    std::set<IDType>        _scriptSet[2];
    std::vector<IDType>     _scriptVec[2];
    std::vector<PlayerPtr>  _playerPtrVec[2];
    
   
    std::vector<UnitAction>       _allScriptMoves[2][PlayerModels::Size];

    std::vector<UnitAction> & getMoves(const IDType & player, const IDType & actualScript);

    UnitAction & getMove(const IDType & player, const IDType & unitIndex, const IDType & actualScript);

public:

    UnitScriptData();

    void calculateMoves(const IDType & player, MoveArray & moves, GameState & state, std::vector<UnitAction> & moveVec);
    void setUnitScript(const IDType & player, const int & id, const IDType & script);
    void setUnitScript(const Unit & unit, const IDType & script);

    const IDType &      getUnitScript(const IDType & player, const int & id) const;
    const IDType &      getUnitScript(const Unit & unit) const;
    const IDType &      getScript(const IDType & player, const size_t & index);
    const PlayerPtr &   getPlayerPtr(const IDType & player, const size_t & index);
    const size_t        getNumScripts(const IDType & player) const;
};
}