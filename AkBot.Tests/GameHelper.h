#pragma once
#include <BWAPI\Client\GameData.h>

namespace AKBot
{
	namespace Tests
	{
		void setP2PForces(BWAPI::GameData* gameData);
		void setPlayers(BWAPI::GameData& gameData, int playersCount);
	}
}