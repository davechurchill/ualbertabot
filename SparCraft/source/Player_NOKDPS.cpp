#include "Player_NOKDPS.h"

using namespace SparCraft;

Player_NOKDPS::Player_NOKDPS (const size_t & playerID) 
{
	_playerID = playerID;
}

void Player_NOKDPS::getMoves(const GameState & state, std::vector<Action> & moveVec)
{
    MoveArray moves;
    ActionGenerators::GenerateCompassActions(state, _playerID, moves);

    moveVec.clear();
	size_t enemy(state.getEnemy(_playerID));

	Array<int, Constants::Max_Units> hpRemaining;

	for (size_t u(0); u<state.numUnits(enemy); ++u)
	{
		hpRemaining[u] = state.getUnit(enemy,u).currentHP();
	}

	for (size_t u(0); u<moves.numUnits(); ++u)
	{
		bool foundAction						(false);
		size_t actionMoveIndex					(0);
		double actionHighestDPS					(0);
		size_t closestMoveIndex					(0);
		unsigned long long closestMoveDist		(std::numeric_limits<unsigned long long>::max());
		
		const Unit & ourUnit				(state.getUnit(_playerID, u));
		const Unit & closestUnit(ourUnit.canHeal() ? AITools::GetClosestOurUnit(state, _playerID, u) : ourUnit.type().isDetector() ? 
            AITools::GetClosestEnemyUnit(state, _playerID, u, false) : AITools::GetClosestEnemyUnit(state,_playerID, u, true));
		
		for (size_t m(0); m<moves.numMoves(u); ++m)
		{
			const Action move						(moves.getMove(u, m));
				
			if ((move.type() == ActionTypes::ATTACK) && (hpRemaining[move.getTargetID()] > 0))
			{
				const Unit & target				(state.getUnit(state.getEnemy(move.getPlayerID()), move.getTargetID()));
				double dpsHPValue =				(target.dpf() / hpRemaining[move.getTargetID()]);

				if (dpsHPValue > actionHighestDPS)
				{
					actionHighestDPS = dpsHPValue;
					actionMoveIndex = m;
					foundAction = true;
				}

                if (move.getTargetID() >= state.numUnits(enemy))
                {
                    int e = enemy;
                    int pl = _playerID;
                    printf("wtf\n");
                }
			}
			else if (move.type() == ActionTypes::HEAL)
			{
				const Unit & target				(state.getUnit(move.getPlayerID(), move.getTargetID()));
				double dpsHPValue =				(target.dpf() / hpRemaining[move.getTargetID()]);

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

		Action theMove(moves.getMove(u, actionMoveIndex));
		if (theMove.type() == ActionTypes::ATTACK)
		{
			hpRemaining[theMove.getTargetID()] -= state.getUnit(_playerID, theMove.getID()).damage();
		}
			
		moveVec.push_back(moves.getMove(u, bestMoveIndex));
	}
}

PlayerPtr Player_NOKDPS::clone()
{
    return PlayerPtr(new Player_NOKDPS(*this));
}