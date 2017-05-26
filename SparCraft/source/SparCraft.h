#pragma once

#ifndef WIN32
#define APIENTRY
#define APIENTRYP
#endif

#include "Common.h"
#include "PlayerProperties.h"
#include "UnitProperties.h"
#include "Player.h"
#include "AllPlayers.h"
#include "Game.h"
#include "GameState.h"
#include "Config.h"
#include "AIParameters.h"
#include "Eval.h"
#include "AITools.h"
#include "JSONTools.h"

namespace SparCraft
{
    void init();
}
