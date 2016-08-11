#include "GameStateUnitData.h"

using namespace SparCraft;

GameStateUnitData::GameStateUnitData()
{
    
}

const Unit & GameStateUnitData::getUnit(const PlayerID & player, const UnitID & index) const
{ 
    SPARCRAFT_ASSERT(player < Constants::Num_Players, "player exceeds capacity: player=%d, total players=%d", player, Constants::Num_Players);
    SPARCRAFT_ASSERT(index < numUnits(player), "UnitIndex exceeds live Unit capacity: UnitIndex=%d, capacity=%d", index, numUnits(player));
    SPARCRAFT_ASSERT(index < _allUnits.size(), "UnitIndex exceeds all Unit capacity: UnitIndex=%d, capacity=%d", index, _allUnits.size());

    return _allUnits[_liveUnitIDs[player][index]];
}

Unit & GameStateUnitData::getUnit(const PlayerID & player, const UnitID & index)
{
    SPARCRAFT_ASSERT(player < Constants::Num_Players, "player exceeds capacity: player=%d, total players=%d", player, Constants::Num_Players);
    SPARCRAFT_ASSERT(index < numUnits(player), "UnitIndex exceeds live Unit capacity: UnitIndex=%d, capacity=%d", index, numUnits(player));
    SPARCRAFT_ASSERT(index < _allUnits.size(), "UnitIndex exceeds all Unit capacity: UnitIndex=%d, capacity=%d", index, _allUnits.size());

    return _allUnits[_liveUnitIDs[player][index]];
}
    
const UnitID GameStateUnitData::numUnits(const PlayerID & player) const
{
    SPARCRAFT_ASSERT(player < Constants::Num_Players, "player exceeds capacity: player=%d", player);

    return (UnitID)_liveUnitIDs[player].size();
}

const Unit & GameStateUnitData::getUnitByID(const UnitID & id) const
{
    SPARCRAFT_ASSERT(id < _allUnits.size(), "id exceeds capacity: id=%d, capacity=%d", id, _allUnits.size());

    return _allUnits[id];
}

Unit & GameStateUnitData::getUnitByID(const UnitID & id)
{
    SPARCRAFT_ASSERT(id < _allUnits.size(), "id exceeds capacity: id=%d, capacity=%d", id, _allUnits.size());

    return _allUnits[id];
}

Unit & GameStateUnitData::addUnit(const Unit & unit)
{
    _allUnits.push_back(unit);
    _allUnits.back().setUnitID(_allUnits.size()-1);

    Unit & unitInVector = _allUnits.back();

    _liveUnitIDs[unitInVector.getPlayerID()].push_back(unitInVector.getID());
    
    return unitInVector;
}

void GameStateUnitData::removeUnit(const PlayerID & player, const UnitID & unitIndex)
{
    SPARCRAFT_ASSERT(player < Constants::Num_Players, "player exceeds capacity: player=%d, total players=%d", player, Constants::Num_Players);    
    SPARCRAFT_ASSERT(unitIndex < numUnits(player), "UnitIndex=%d, size=%d", unitIndex, numUnits(player));

    _liveUnitIDs[player].erase(_liveUnitIDs[player].begin() + unitIndex);
}

void GameStateUnitData::removeUnitByID(const UnitID & unitID)
{
    const PlayerID player = getUnitByID(unitID).getPlayerID();
    const UnitID units = numUnits(player);
    
    for (UnitID c(0); c < units; ++c)
    {
        if (_liveUnitIDs[player][c] == unitID)
        {
            removeUnit(player, c);
            return;
        }
    }

    SPARCRAFT_ASSERT(false, "Tried to remove a Unit that didn't exist: %d", unitID);
}

void GameStateUnitData::killUnit(const UnitID & UnitID)
{
    Unit & Unit = getUnitByID(UnitID);

    removeUnitByID(UnitID);
}

const UnitIDVector & GameStateUnitData::getUnitIDs(const PlayerID & player) const
{
    return _liveUnitIDs[player];
}