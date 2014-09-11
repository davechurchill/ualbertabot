#include "Player_Cluster.h"

using namespace SparCraft;

Player_Cluster::Player_Cluster (const IDType & playerID) 
{
	_playerID = playerID;
}

void Player_Cluster::getMoves(GameState & state, const MoveArray & moves, std::vector<UnitAction> & moveVec)
{
    moveVec.clear();
	IDType enemy(state.getEnemy(_playerID));

    // compute the centroid of our unit cluster
    Position avgPos(0,0);
    for (size_t u(0); u<state.numUnits(_playerID); ++u)
    {
        avgPos.addPosition(state.getUnit(_playerID, u).pos());
    }

    avgPos.scalePosition(1.0f / state.numUnits(_playerID));

	for (IDType u(0); u<moves.numUnits(); ++u)
	{
		bool foundMoveAction					(false);
		double actionHighestDPS					(0);
		size_t closestMoveIndex					(0);
		unsigned long long closestMoveDist		(std::numeric_limits<unsigned long long>::max());
		
		const Unit & ourUnit				(state.getUnit(_playerID, u));
		
		for (size_t m(0); m<moves.numMoves(u); ++m)
		{
			const UnitAction move						(moves.getMove(u, m));
				
			if (move.type() == UnitActionTypes::MOVE)
			{
				Position ourDest				(ourUnit.x() + Constants::Move_Dir[move._moveIndex][0], 
												 ourUnit.y() + Constants::Move_Dir[move._moveIndex][1]);
                size_t dist						(avgPos.getDistanceSq(ourDest));

				if (dist < closestMoveDist)
				{
					closestMoveDist = dist;
					closestMoveIndex = m;
                    foundMoveAction = true;
				}
			}
		}

		size_t bestMoveIndex(foundMoveAction ? closestMoveIndex : 0);
        			
		moveVec.push_back(moves.getMove(u, bestMoveIndex));
	}
}
