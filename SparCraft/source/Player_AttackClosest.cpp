#include "Player_AttackClosest.h"

using namespace SparCraft;

Player_AttackClosest::Player_AttackClosest(const size_t & playerID)
{
    _playerID = playerID;
}

void Player_AttackClosest::getMoves(const GameState & state, std::vector<Action> & moveVec)
{
    MoveArray moves;
    ActionGenerators::GenerateCompassActions(state, _playerID, moves);

    moveVec.clear();
    for (size_t u(0); u<moves.numUnits(); ++u)
    {
        bool foundAction = false;
        size_t actionMoveIndex = 0;
        size_t closestMoveIndex = 0;
        unsigned long long actionDistance(std::numeric_limits<unsigned long long>::max());
        unsigned long long closestMoveDist(std::numeric_limits<unsigned long long>::max());

        SPARCRAFT_ASSERT(moves.numMoves(u) > 0, "0 moves found for unit");

        const Unit & ourUnit = state.getUnitByID(moves.getMove(u, 0).getID());
        const Unit & closestUnit = ourUnit.canHeal() ? AITools::GetClosestOurUnit(state, _playerID, u) : AITools::GetClosestEnemyUnit(state, _playerID, u);

        for (size_t m(0); m<moves.numMoves(u); ++m)
        {
            const Action action = moves.getMove(u, m);

            if (action.type() == ActionTypes::ATTACK)
            {
                const Unit & target = state.getUnitByID(action.getTargetID());
                size_t dist = ourUnit.getDistanceSqToUnit(target,state.getTime());

                if (dist < actionDistance)
                {
                    actionDistance = dist;
                    actionMoveIndex = m;
                    foundAction = true;
                }
            }

            if (action.type() == ActionTypes::HEAL)
            {
                const Unit & target = state.getUnitByID(action.getTargetID());
                size_t dist = ourUnit.getDistanceSqToUnit(target,state.getTime());

                if (dist < actionDistance)
                {
                    actionDistance = dist;
                    actionMoveIndex = m;
                    foundAction = true;
                }
            }
            else if (action.type() == ActionTypes::RELOAD)
            {
                if (ourUnit.canAttackTarget(closestUnit,state.getTime()))
                {
                    closestMoveIndex = m;
                    break;
                }
            }
            else if (action.type() == ActionTypes::MOVE)
            {
                Position ourDest(ourUnit.x() + Constants::Move_Dir[action.getTargetID()][0], ourUnit.y() + Constants::Move_Dir[action.getTargetID()][1]);
                size_t dist(closestUnit.getDistanceSqToPosition(ourDest,state.getTime()));

                if (dist < closestMoveDist)
                {
                    closestMoveDist = dist;
                    closestMoveIndex = m;
                }
            }
        }

        size_t bestMoveIndex = foundAction ? actionMoveIndex : closestMoveIndex;

        Action toAdd = moves.getMove(u, bestMoveIndex);
        
        SPARCRAFT_ASSERT(toAdd.type() != ActionTypes::NONE, "Adding NONE action type");

        moveVec.push_back(toAdd);
    }
}

PlayerPtr Player_AttackClosest::clone()
{
    return PlayerPtr(new Player_AttackClosest(*this));
}