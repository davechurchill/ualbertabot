#pragma once

#include "Common.h"
#include <BWAPI/Unit.h>
#include "../../SparCraft/source/SparCraft.h"
#include "ArenaPlayer.h"
#include "ScreenCanvas.h"

namespace UAlbertaBot
{
    
class ArenaPlayer_SparCraft : public ArenaPlayer
{
    SparCraft::GameState    GetSparCraftState() const;
    SparCraft::Unit         GetSparCraftUnit(BWAPI::Unit unit) const;
    SparCraft::Move         GetSparCraftPlayerMove(const SparCraft::GameState & state, const size_t & playerID) const;
    size_t                  GetSparCraftPlayerID(BWAPI::Player player) const;
    void                    DoSparCraftMove(AKBot::ScreenCanvas& canvas, const SparCraft::GameState & state, const SparCraft::Move & move) const;
    void                    DrawSparCraftState(AKBot::ScreenCanvas& canvas, const SparCraft::GameState & state, int x, int y) const;
    void                    DrawSparCraftMove(AKBot::ScreenCanvas& canvas, const SparCraft::GameState & state, const SparCraft::Move & move) const;

    int                     GetTimeSinceLastAttack(BWAPI::Unit unit) const;
    int                     GetTimeCanAttack(BWAPI::Unit unit) const;
    int                     GetTimeCanMove(BWAPI::Unit unit) const;

    void                    PlaySparCraftSimulation(const SparCraft::GameState & state);

public:

	ArenaPlayer_SparCraft();

    void onStart();
    void onFrame();
    void onBattleBegin();
    void onBattleEnd();
};
}