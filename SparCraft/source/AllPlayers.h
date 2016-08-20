#pragma once

#include "Common.h"
#include "Player.h"
#include <memory>

// search-based players
#include "Player_AlphaBeta.h"
#include "Player_PortfolioGreedySearch.h"
#include "Player_UCT.h"

// script-based players
#include "Player_AttackClosest.h"
#include "Player_Random.h"
#include "Player_Script.h"

namespace SparCraft
{
    
typedef std::shared_ptr<Player> PlayerPtr;

}