#pragma once

#include "Common.h"
#include "PlayerProperties.h"
#include "UnitProperties.h"
#include "Player.h"
#include "AllPlayers.h"
#include "Game.h"
#include "GameState.h"
#include "SearchExperiment.h"
#include "AnimationFrameData.h"

#ifdef USING_VISUALIZATION_LIBRARIES
	#include "Display.h"
#endif

namespace SparCraft
{
    void init();
}