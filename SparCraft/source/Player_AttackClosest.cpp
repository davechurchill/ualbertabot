#include "Player_AttackClosest.h"

using namespace SparCraft;

Player_AttackClosest::Player_AttackClosest (const IDType & playerID) 
{
	_playerID = playerID;
}

void Player_AttackClosest::getMoves(GameState & state, const MoveArray & moves, std::vector<UnitAction> & moveVec)
{
    moveVec.clear();
	for (IDType u(0); u<moves.numUnits(); ++u)
	{
		bool foundUnitAction				(false);
		size_t actionMoveIndex				(0);
		size_t closestMoveIndex				(0);
		unsigned long long actionDistance	(std::numeric_limits<unsigned long long>::max());
		unsigned long long closestMoveDist	(std::numeric_limits<unsigned long long>::max());

		const Unit & ourUnit				(state.getUnit(_playerID, u));
		const Unit & closestUnit			(ourUnit.canHeal() ? state.getClosestOurUnit(_playerID, u) : state.getClosestEnemyUnit(_playerID, u));

		for (size_t m(0); m<moves.numMoves(u); ++m)
		{
			const UnitAction move					(moves.getMove(u, m));
				
			if (move.type() == UnitActionTypes::ATTACK)
			{
				const Unit & target			(state.getUnit(state.getEnemy(move.player()), move._moveIndex));
				size_t dist					(ourUnit.getDistanceSqToUnit(target, state.getTime()));

				if (dist < actionDistance)
				{
					actionDistance = dist;
					actionMoveIndex = m;
					foundUnitAction = true;
				}
			}
			if (move.type() == UnitActionTypes::HEAL)
			{
				const Unit & target			(state.getUnit(move.player(), move._moveIndex));
				size_t dist					(ourUnit.getDistanceSqToUnit(target, state.getTime()));

				if (dist < actionDistance)
				{
					actionDistance = dist;
					actionMoveIndex = m;
					foundUnitAction = true;
				}
			}
			else if (move.type() == UnitActionTypes::RELOAD)
			{
				if (ourUnit.canAttackTarget(closestUnit, state.getTime()))
				{
					closestMoveIndex = m;
					break;
				}
			}
			else if (move.type() == UnitActionTypes::MOVE)
			{
				Position ourDest			(ourUnit.x() + Constants::Move_Dir[move._moveIndex][0], 
											 ourUnit.y() + Constants::Move_Dir[move._moveIndex][1]);
				size_t dist					(closestUnit.getDistanceSqToPosition(ourDest, state.getTime()));

				if (dist < closestMoveDist)
				{
					closestMoveDist = dist;
					closestMoveIndex = m;
				}
			}
		}

		size_t bestMoveIndex(foundUnitAction ? actionMoveIndex : closestMoveIndex);

		moveVec.push_back(moves.getMove(u, bestMoveIndex));
	}
}
