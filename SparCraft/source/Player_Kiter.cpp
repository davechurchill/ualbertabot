#include "Player_Kiter.h"

using namespace SparCraft;

Player_Kiter::Player_Kiter (const IDType & playerID) 
{
	_playerID = playerID;
}

void Player_Kiter::getMoves(GameState & state, const MoveArray & moves, std::vector<UnitAction> & moveVec)
{
    moveVec.clear();
	for (IDType u(0); u<moves.numUnits(); ++u)
	{
		bool foundUnitAction						(false);
		IDType actionMoveIndex					(0);
		IDType furthestMoveIndex				(0);
		size_t furthestMoveDist					(0);
		IDType closestMoveIndex					(0);
		int actionDistance						(std::numeric_limits<int>::max());
		unsigned long long closestMoveDist		(std::numeric_limits<unsigned long long>::max());

		const Unit & ourUnit					(state.getUnit(_playerID, u));
		const Unit & closestUnit				(ourUnit.canHeal() ? state.getClosestOurUnit(_playerID, u) : state.getClosestEnemyUnit(_playerID, u));

		for (IDType m(0); m<moves.numMoves(u); ++m)
		{
			const UnitAction move						(moves.getMove(u, m));
				
			if (move.type() == UnitActionTypes::ATTACK)
			{
				const Unit & target				(state.getUnit(state.getEnemy(move.player()), move._moveIndex));
				PositionType dist				(ourUnit.getDistanceSqToUnit(target, state.getTime()));

				if (dist < actionDistance)
				{
					actionDistance = dist;
					actionMoveIndex = m;
					foundUnitAction = true;
				}
			}
			else if (move.type() == UnitActionTypes::HEAL)
			{
				const Unit & target				(state.getUnit(move.player(), move._moveIndex));
				PositionType dist				(ourUnit.getDistanceSqToUnit(target, state.getTime()));

				if (dist < actionDistance)
				{
					actionDistance = dist;
					actionMoveIndex = m;
					foundUnitAction = true;
				}
			}
			else if (move.type() == UnitActionTypes::MOVE)
			{
				Position ourDest				(ourUnit.x() + Constants::Move_Dir[move._moveIndex][0], 
												 ourUnit.y() + Constants::Move_Dir[move._moveIndex][1]);
				size_t dist						(closestUnit.getDistanceSqToPosition(ourDest, state.getTime()));

				if (dist > furthestMoveDist)
				{
					furthestMoveDist = dist;
					furthestMoveIndex = m;
				}

				if (dist < closestMoveDist)
				{
					closestMoveDist = dist;
					closestMoveIndex = m;
				}
			}
		}

		// the move we will be returning
		size_t bestMoveIndex(0);

		// if we have an attack move we will use that one
		if (foundUnitAction)
		{
			bestMoveIndex = actionMoveIndex;
		}
		// otherwise use the closest move to the opponent
		else
		{
			// if we are in attack range of the unit, back up
			if (ourUnit.canAttackTarget(closestUnit, state.getTime()))
			{
				bestMoveIndex = furthestMoveIndex;
			}
			// otherwise get back into the fight
			else
			{
				bestMoveIndex = closestMoveIndex;
			}
		}
			
		moveVec.push_back(moves.getMove(u, bestMoveIndex));
	}
}
