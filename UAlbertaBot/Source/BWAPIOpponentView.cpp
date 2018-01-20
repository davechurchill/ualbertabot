#include "BWAPIOpponentView.h"
#include <BWAPI\Game.h>

AKBot::BWAPIOpponentView::BWAPIOpponentView(BWAPI::Game* game): _game(game)
{
}

BWAPI::Player AKBot::BWAPIOpponentView::self() const
{
	return _game->self();
}

BWAPI::Player AKBot::BWAPIOpponentView::defaultEnemy() const
{
	const auto defaultEnemy = _game->enemy();
	if (defaultEnemy == nullptr)
	{
		return nullptr;
	}

	if (defaultEnemy->leftGame() || defaultEnemy->isDefeated())
	{
		for (const auto& enemy : _game->enemies())
		{
			if (!enemy->leftGame() && !enemy->isDefeated())
			{
				return enemy;
			}
		}
	}

	return defaultEnemy;
}

const BWAPI::Playerset & AKBot::BWAPIOpponentView::enemies() const
{
	return _game->enemies();
}
