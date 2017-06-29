#pragma once

#include "ArenaPlayer.h"

namespace UAlbertaBot
{
    
class ArenaPlayer_AttackClosest : public ArenaPlayer
{
    
    BWAPI::Unit getClosestEnemyUnit(BWAPI::Unit ourUnit) const;

public:

	ArenaPlayer_AttackClosest();

    void onStart();
    void onFrame();
    void onBattleBegin();
    void onBattleEnd();
};
}