#include "BWAPIReplayOpponentView.h"
#include <BWAPI\Game.h>
#include <algorithm>
#include <iterator>

AKBot::BWAPIReplayOpponentView::BWAPIReplayOpponentView(BWAPI::Game* game): _enemies()
{
	auto players = game->getPlayers();
	auto selfByName = std::find_if(players.begin(), players.end(), [](const auto item) {
		return item->getName() == "Andrey Kurdiumov";
	});
	_self = *selfByName;
	std::copy_if(players.begin(), players.end(), std::inserter(_enemies, _enemies.begin()), [](const auto item)
	{
		return item->getName() != "Andrey Kurdiumov" && !item->isNeutral();
	});
}

BWAPI::Player AKBot::BWAPIReplayOpponentView::self() const
{
	return _self;
}

BWAPI::Player AKBot::BWAPIReplayOpponentView::defaultEnemy() const
{
	for (const auto& enemy : _enemies)
	{
		if (!enemy->leftGame() && !enemy->isDefeated())
		{
			return enemy;
		}
	}

	return nullptr;
}

const BWAPI::Playerset & AKBot::BWAPIReplayOpponentView::enemies() const
{
	return _enemies;
}
