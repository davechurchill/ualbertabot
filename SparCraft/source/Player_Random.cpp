#include "Player_Random.h"

using namespace SparCraft;

Player_Random::Player_Random (const PlayerID & playerID)
	: rand(0, std::numeric_limits<int>::max(), Constants::Seed_Player_Random_Time ? static_cast<unsigned int>(std::time(0)) : 0)
{
	_playerID = playerID;
}

void Player_Random::getMoves(const GameState & state, std::vector<Action> & moveVec)
{
    MoveArray moves;
    state.generateMoves(moves, _playerID);
	for (size_t u(0); u<moves.numUnits(); u++)
	{
		moveVec.push_back(moves.getMove(u, rand.nextInt() % moves.numMoves(u)));
	}
}

PlayerPtr Player_Random::clone()
{
    return PlayerPtr(new Player_Random(*this));
}