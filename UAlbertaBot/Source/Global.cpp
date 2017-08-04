#include "Global.h"
#include "UAlbertaBot_Tournament.h"

using namespace UAlbertaBot;

UAlbertaBot_Tournament * uabModule = nullptr;

void Global::SetModule(UAlbertaBot_Tournament * module)
{
    uabModule = module;
}

WorkerManager & Global::Workers()
{
    return uabModule->Workers();
}

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