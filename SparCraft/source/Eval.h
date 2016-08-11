#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"

namespace SparCraft
{
namespace Eval
{
    PlayerID  PerformPlayout(const GameState & state, const PlayerPtr & p1, const PlayerPtr & p2);
}
}
