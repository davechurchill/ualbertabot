#include "Player_Script.h"

using namespace SparCraft;

Player_Script::Player_Script (const size_t & playerID, const ScriptPlayerPolicy & policy)
{
	_playerID = playerID;
    _playerCentersCalculated[0] = false;
    _playerCentersCalculated[1] = false;
}

void Player_Script::getMove(const GameState & state, Move & move)
{
    move.clear();
	const size_t enemy = state.getEnemy(_playerID);
    
    std::vector<size_t> unitsInAttackRange;

    for (size_t u(0); u < state.numUnits(_playerID); ++u)
    {
        const Unit & unit = state.getUnit(_playerID, u);

        // If the unit can't act yet, then go to the next one
        if (unit.firstTimeFree() > state.getTime())
        {
            continue;
        }

        getEnemyUnitsInAttackRange(unit, state, unitsInAttackRange);

        // If we can't currently attack any enemy units
        if (unitsInAttackRange.empty())
        {
            const ScriptPolicy & policy = _policy.getOutOfRangePolicy();

            SPARCRAFT_ASSERT(policy.getActionType() == PolicyAction::Move, "We can only Move if out of range");


        }
        // We are able to attack at least one enemy unit
        else
        {
        
        }
    }
}

const Position & Player_Script::getPlayerCenter(const GameState & state, const size_t & playerID)
{
    if (!_playerCentersCalculated[playerID])
    {
        AITools::CalculatePlayerCenter(state, playerID);
        _playerCentersCalculated[playerID] = true;
    }

    return _playerCenters[playerID];
}

void Player_Script::getEnemyUnitsInAttackRange(const Unit & myUnit, const GameState & state, std::vector<size_t> & unitIDs)
{
    const size_t enemy = state.getEnemy(_playerID);
    unitIDs.clear();
    
    for (size_t u(0); u < state.numUnits(enemy); ++u)
    {
        const Unit & enemyUnit = state.getUnit(enemy, u);
        
        if (myUnit.canAttackTarget(enemyUnit, state.getTime()))
        {
            unitIDs.push_back(enemyUnit.getID());
        }
    }
}

PlayerPtr Player_Script::clone()
{
    return PlayerPtr(new Player_Script(*this));
}