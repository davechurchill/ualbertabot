#pragma once

#include "ArenaPlayer.h"
#include <BWAPI/Unit.h>

namespace UAlbertaBot
{
    
class ArenaPlayer_AttackClosest : public ArenaPlayer
{
    
    BWAPI::Unit getClosestEnemyUnit(BWAPI::Unit ourUnit) const;

public:

	ArenaPlayer_AttackClosest();

    void onStart();
    void onFrame(int currentFrame);
    void onBattleBegin();
    void onBattleEnd();
};
}