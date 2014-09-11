#pragma once

#include "Common.h"
#include "BWAPI.h"
#include "GameCommander.h"
#include "UnitCommandManager.h"
#include "../../SparCraft/Source/SparCraft.h"
#include <boost/shared_ptr.hpp>

namespace UAlbertaBot
{
class SparCraftManager
{
	bool gameOver;
    
public:

	SparCraftManager();

	SparCraft::GameState				extractGameState();
		
	const SparCraft::IDType				getPlayerID(BWAPI::Player * player) const;
	const bool							isCombatUnit(BWAPI::Unit * unit) const;
	void                        		getMoves(SparCraft::GameState & state, std::vector<SparCraft::UnitAction> & moveVec);

	void update();
	void onStart();
	
	void doUnitMove(SparCraft::GameState & currentState, SparCraft::Unit & unit, SparCraft::UnitAction & move);
	void drawUnitMove(SparCraft::GameState & currentState, SparCraft::Unit & unit, SparCraft::UnitAction & move);
	void drawUnitCooldown(BWAPI::Unit * unit);
	void drawUnitHP(BWAPI::Unit * unit);
	void drawSearchResults(int x, int y);
	void drawAttackDebug();
	void performSparCraft();

    SparCraft::Unit getSparCraftUnit(const BWAPI::Unit * bwapiUnit, BWAPI::Game * game);
	BWAPI::Unit * getUnit(SparCraft::Unit & unit);
	const std::pair<int, int> getUnitCooldown(BWAPI::Unit * unit, SparCraft::Unit & u) const;
};
}