#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"

namespace SparCraft
{
namespace AITools
{
    const Unit & GetClosestEnemyUnit(const GameState & state, const size_t & player, const size_t & unitIndex, bool checkCloaked = false);
    const Unit & GetClosestOurUnit(const GameState & state, const size_t & player, const size_t & unitIndex);

    void            StatePrint(const GameState & state, int indent = 0);
    std::string     StateToString(const GameState & state);
    std::string     StateToStringCompact(const GameState & state);
}
}
