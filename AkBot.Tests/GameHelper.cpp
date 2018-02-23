#include "GameHelper.h"

void AKBot::Tests::setP2PForces(BWAPI::GameData* gameData)
{
	static char forceNames[][32] = {
		"Players",
		"Observers",
		"Force3",
		"Force4",
		""
	};
	gameData->forceCount = 3;
	for (auto i = 0; i < gameData->forceCount; i++)
	{
		strcpy_s(gameData->forces[i].name, forceNames[i]);
	}
}

void AKBot::Tests::setPlayers(BWAPI::GameData& gameData, int playersCount)
{
	static char neutralName[25] = "Neutral";
	static char playerNames[][25] = {
		"Player1",
		"Player2",
		"Player3",
		"Player4",
		"Player5",
		"Player6",
		"Player7",
		"Player8",
		"Player9",
		"Player10",
		"Player11",
		"Player12"
	};

	for (auto i = 0; i < playersCount; i++)
	{
		strcpy_s(gameData.players[i].name, playerNames[i]);
		for (auto j = 0; j < playersCount; j++)
		{
			if (j != i)
			{
				gameData.players[i].isEnemy[j] = true;
			}
		}
	}

	gameData.players[playersCount].isNeutral = true;
	strcpy_s(gameData.players[playersCount].name, neutralName);
	gameData.playerCount = playersCount + 1;
	gameData.neutral = playersCount;
}