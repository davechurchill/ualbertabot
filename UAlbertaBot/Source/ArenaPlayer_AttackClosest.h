#pragma once

#include "ArenaPlayer.h"
#include <BWAPI/Unit.h>
#include "OpponentView.h"

namespace UAlbertaBot
{
	using AKBot::OpponentView;

class ArenaPlayer_AttackClosest : public ArenaPlayer
{
	std::shared_ptr<OpponentView> _opponentView;
    BWAPI::Unit getClosestEnemyUnit(BWAPI::Unit ourUnit) const;

public:

	ArenaPlayer_AttackClosest(std::shared_ptr<OpponentView> opponentView);

    void onStart();
    void onFrame(int currentFrame);
    void onBattleBegin();
    void onBattleEnd();
};
}