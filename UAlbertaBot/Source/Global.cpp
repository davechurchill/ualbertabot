#include "Global.h"
#include "BWAPI.h"

using namespace UAlbertaBot;

BWAPI::Player Global::getEnemy()
{
	const auto defaultEnemy = BWAPI::Broodwar->enemy();
	if (defaultEnemy == nullptr)
	{
		return nullptr;
	}

	if (defaultEnemy->leftGame() || defaultEnemy->isDefeated())
	{
		for (const auto& enemy : BWAPI::Broodwar->enemies())
		{
			if (!enemy->leftGame() && !enemy->isDefeated())
			{
				return enemy;
			}
		}
	}

	return defaultEnemy;
}