#include "GameStateUnitData.h"

using namespace SparCraft;

GameStateUnitData::GameStateUnitData()
{

}

const Unit & GameStateUnitData::getUnit(const size_t & player, const size_t & index) const
{ 
    SPARCRAFT_ASSERT(player < Players::Num_Players, "Player exceeds capacity: player=%d", player);
    SPARCRAFT_ASSERT(index < numUnits(player), "UnitIndex exceeds live Unit capacity: UnitIndex=%d, capacity=%d", index, numUnits(player));

    return _allUnits[_liveUnitIDs[player][index]];
}

Unit & GameStateUnitData::getUnit(const size_t & player, const size_t & index)
{
    SPARCRAFT_ASSERT(player < Players::Num_Players, "Player exceeds capacity: player=%d", player);
    SPARCRAFT_ASSERT(index < numUnits(player), "UnitIndex exceeds live Unit capacity: UnitIndex=%d, capacity=%d", index, numUnits(player));

    return _allUnits[_liveUnitIDs[player][index]];
}
    
const size_t GameStateUnitData::numUnits(const size_t & player) const
{
    SPARCRAFT_ASSERT(player < Players::Num_Players, "Player exceeds capacity: player=%d", player);

    return _liveUnitIDs[player].size();
}

const Unit & GameStateUnitData::getUnitByID(const size_t & id) const
{
    SPARCRAFT_ASSERT(id < _allUnits.size(), "Unit id exceeds capacity: id=%d, capacity=%d", id, _allUnits.size());

    return _allUnits[id];
}

Unit & GameStateUnitData::getUnitByID(const size_t & id)
{
    SPARCRAFT_ASSERT(id < _allUnits.size(), "Unit id exceeds capacity: id=%d, capacity=%d", id, _allUnits.size());

    return _allUnits[id];
}

Unit & GameStateUnitData::addUnit(const Unit & unit)
{
    _allUnits.push_back(unit);
    _allUnits.back().setUnitID(_allUnits.size()-1);

    Unit & unitInVector = _allUnits.back();

	auto playerId = unitInVector.getPlayerID();
	if (playerId != Players::Player_None)
	{
		auto& playerLiveUnits = _liveUnitIDs[playerId];
		playerLiveUnits.push_back(unitInVector.getID());
	}

	return unitInVector;
}

void GameStateUnitData::removeUnit(const size_t & player, const size_t & unitIndex)
{
    SPARCRAFT_ASSERT(player < Players::Num_Players, "player exceeds capacity: player=%d, total players=%d", player, Players::Num_Players);    
    SPARCRAFT_ASSERT(unitIndex < numUnits(player), "UnitIndex=%d, size=%d", unitIndex, numUnits(player));

    _liveUnitIDs[player].erase(_liveUnitIDs[player].begin() + unitIndex);
}

void GameStateUnitData::removeUnitByID(const size_t & unitID)
{
    const size_t player = getUnitByID(unitID).getPlayerID();
    const size_t units = numUnits(player);
    
    for (size_t c(0); c < units; ++c)
    {
        if (_liveUnitIDs[player][c] == unitID)
        {
            removeUnit(player, c);
            return;
        }
    }

    SPARCRAFT_ASSERT(false, "Tried to remove a Unit that didn't exist: %d", unitID);
}

void GameStateUnitData::killUnit(const size_t & UnitID)
{
    Unit & Unit = getUnitByID(UnitID);

    removeUnitByID(UnitID);
}

const std::vector<size_t> & GameStateUnitData::getUnitIDs(const size_t & player) const
{
    return _liveUnitIDs[player];
}

const std::vector<Unit> & GameStateUnitData::getAllUnits() const
{
    return _allUnits;
}

std::vector<Unit> & GameStateUnitData::getAllUnits()
{
	return _allUnits;
}