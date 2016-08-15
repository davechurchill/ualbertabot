#include "Player_AttackDPS.h"

using namespace SparCraft;

Player_AttackDPS::Player_AttackDPS (const size_t & playerID) 
{
	_playerID = playerID;
}

void Player_AttackDPS::getMove(const GameState & state, Move & move)
{
    MoveArray moves;
    ActionGenerators::GenerateCompassActions(state, _playerID, moves);

    move.clear();
	for (size_t u(0); u<moves.numUnits(); ++u)
	{
		bool foundAction						(false);
		size_t actionMoveIndex					(0);
		size_t closestMoveIndex					(0);
		double actionHighestDPS					(0);
		unsigned long long closestMoveDist		(std::numeric_limits<unsigned long long>::max());
		
		const Unit & ourUnit					(state.getUnit(_playerID, u));
		const Unit & closestUnit				(ourUnit.canHeal() ? AITools::GetClosestOurUnit(state, _playerID, u) : AITools::GetClosestEnemyUnit(state, _playerID, u));

		for (size_t m(0); m<moves.numMoves(u); ++m)
		{
			const Action move						(moves.getMove(u, m));
				
			if (move.type() == ActionTypes::ATTACK)
			{
				const Unit & target				(state.getUnit(state.getEnemy(move.getPlayerID()), move.getTargetID()));
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
				const Unit & target				(state.getUnit(move.getPlayerID(), move.getTargetID()));
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
				Position ourDest				(ourUnit.x() + Constants::Move_Dir[move.getTargetID()][0], 
												 ourUnit.y() + Constants::Move_Dir[move.getTargetID()][1]);
				size_t dist						(closestUnit.getDistanceSqToPosition(ourDest, state.getTime()));

				if (dist < closestMoveDist)
				{
					closestMoveDist = dist;
					closestMoveIndex = m;
				}
			}
		}

		size_t bestMoveIndex(foundAction ? actionMoveIndex : closestMoveIndex);
			
		move.addAction(moves.getMove(u, bestMoveIndex));
	}
}

PlayerPtr Player_AttackDPS::clone()
{
    return PlayerPtr(new Player_AttackDPS(*this));
}