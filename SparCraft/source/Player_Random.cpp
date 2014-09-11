#include "Player_Random.h"

using namespace SparCraft;

Player_Random::Player_Random (const IDType & playerID)
	: rand(0, std::numeric_limits<int>::max(), 0)
{
	_playerID = playerID;

	if (Constants::Seed_Player_Random_Time)
	{
		rand.seed(static_cast<unsigned int>(std::time(0)));
	}
}

void Player_Random::getMoves(GameState & state, const MoveArray & moves, std::vector<UnitAction> & moveVec)
{
	for (size_t u(0); u<moves.numUnits(); u++)
	{
		moveVec.push_back(moves.getMove(u, rand.nextInt() % moves.numMoves(u)));
	}
}
