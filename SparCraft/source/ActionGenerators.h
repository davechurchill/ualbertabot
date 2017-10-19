#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"

namespace SparCraft
{
namespace ActionGenerators
{
    void GenerateCompassActions(const GameState & state, const size_t & player, MoveArray & moves);
}
}
