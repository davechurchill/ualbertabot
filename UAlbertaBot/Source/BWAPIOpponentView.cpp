#include "BWAPIOpponentView.h"
#include <BWAPI\Game.h>

BWAPI::Player AKBot::BWAPIOpponentView::self() const
{
	return BWAPI::Broodwar->self();
}

BWAPI::Player AKBot::BWAPIOpponentView::defaultEnemy() const
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

BWAPI::Playerset & AKBot::BWAPIOpponentView::enemies() const
{
	return BWAPI::Broodwar->enemies();
}
