#include "UnitScriptData.h"

using namespace SparCraft;

UnitScriptData::UnitScriptData() 
{
}

std::vector<Action> & UnitScriptData::getMoves(const PlayerID & player, const PlayerID & actualScript)
{
    return _allScriptMoves[player][actualScript];
}

Action & UnitScriptData::getMove(const PlayerID & player, const PlayerID & unitIndex, const PlayerID & actualScript)
{
    return _allScriptMoves[player][actualScript][unitIndex];
}

void UnitScriptData::calculateMoves(const PlayerID & player, MoveArray & moves, GameState & state, std::vector<Action> & moveVec)
{
    // generate all script moves for this player at this state and store them in allScriptMoves
    for (size_t scriptIndex(0); scriptIndex<_scriptVec[player].size(); ++scriptIndex)
    {
        // get the associated player pointer
        const PlayerPtr & pp = getPlayerPtr(player, scriptIndex);

        // get the actual script we are working with
        const PlayerID actualScript = getScript(player, scriptIndex);

        // generate the moves inside the appropriate vector
        getMoves(player, actualScript).clear();
        pp->getMoves(state, getMoves(player, actualScript));
    }

    // for each unit the player has to move, populate the move vector with the appropriate script move
    for (size_t unitIndex(0); unitIndex < moves.numUnits(); ++unitIndex)
    {
        // the unit from the state
        const Unit & unit = state.getUnit(player, unitIndex);

        // the move it would choose to do based on its associated script preference
        Action unitMove = getMove(player, unitIndex, getUnitScript(unit));

        // put the unit into the move vector
        moveVec.push_back(unitMove);
    }
}

const PlayerID & UnitScriptData::getUnitScript(const PlayerID & player, const int & id) const
{
    return (*_unitScriptMap[player].find(id)).second;
}
    
const PlayerID & UnitScriptData::getUnitScript(const Unit & unit) const
{
    return getUnitScript(unit.getPlayerID(), unit.getID());
}

const PlayerID & UnitScriptData::getScript(const PlayerID & player, const size_t & index)
{
    return _scriptVec[player][index];
}

const PlayerPtr & UnitScriptData::getPlayerPtr(const PlayerID & player, const size_t & index)
{
    return _playerPtrVec[player][index];
}

const size_t UnitScriptData::getNumScripts(const PlayerID & player) const
{
    return _scriptSet[player].size();
}

void UnitScriptData::setUnitScript(const PlayerID & player, const int & id, const PlayerID & script)
{
    if (_scriptSet[player].find(script) == _scriptSet[player].end())
    {
        _scriptSet[player].insert(script);
        _scriptVec[player].push_back(script);
        _playerPtrVec[player].push_back(PlayerPtr(AllPlayers::getPlayerPtr(player, script)));
    }
        
    _unitScriptMap[player][id] = script;
}

void UnitScriptData::setUnitScript(const Unit & unit, const PlayerID & script)
{
    setUnitScript(unit.getPlayerID(), unit.getID(), script);
}