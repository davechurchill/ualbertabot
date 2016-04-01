#include "Player_AttackDPS.h"

using namespace SparCraft;

Player_AttackDPS::Player_AttackDPS (const IDType & playerID) 
{
	_playerID = playerID;
}

void Player_AttackDPS::getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec)
{
    moveVec.clear();
	for (IDType u(0); u<moves.numUnits(); ++u)
	{
		bool foundAction						(false);
		size_t actionMoveIndex					(0);
		size_t closestMoveIndex					(0);
		double actionHighestDPS					(0);
		unsigned long long closestMoveDist		(std::numeric_limits<unsigned long long>::max());
		
		const Unit & ourUnit					(state.getUnit(_playerID, u));
		const Unit & closestUnit				(ourUnit.canHeal() ? state.getClosestOurUnit(_playerID, u) : state.getClosestEnemyUnit(_playerID, u));

		for (size_t m(0); m<moves.numMoves(u); ++m)
		{
			const Action move						(moves.getMove(u, m));
				
			if (move.type() == ActionTypes::ATTACK)
			{
				const Unit & target				(state.getUnit(state.getEnemy(move.player()), move.index()));
				double dpsHPValue =				(target.dpf() / target.currentHP());

				if (dpsHPValue > actionHighestDPS)
				{
					actionHighestDPS = dpsHPValue;
					actionMoveIndex = m;
					foundAction = true;
				}
			}
			if (move.type() == ActionTypes::HEAL)
			{
				const Unit & target				(state.getUnit(move.player(), move.index()));
				double dpsHPValue =				(target.dpf() / target.currentHP());

				if (dpsHPValue > actionHighestDPS)
				{
					actionHighestDPS = dpsHPValue;
					actionMoveIndex = m;
					foundAction = true;
				}
			}
			else if (move.type() == ActionTypes::RELOAD)
			{
				if (ourUnit.canAttackTarget(closestUnit, state.getTime()))
				{
					closestMoveIndex = m;
					break;
				}
			}
			else if (move.type() == ActionTypes::MOVE)
			{
				Position ourDest				(ourUnit.x() + Constants::Move_Dir[move.index()][0], 
												 ourUnit.y() + Constants::Move_Dir[move.index()][1]);
				size_t dist						(closestUnit.getDistanceSqToPosition(ourDest, state.getTime()));

				if (dist < closestMoveDist)
				{
					closestMoveDist = dist;
					closestMoveIndex = m;
				}
			}
		}

		size_t bestMoveIndex(foundAction ? actionMoveIndex : closestMoveIndex);
			
		moveVec.push_back(moves.getMove(u, bestMoveIndex));
	}
}
