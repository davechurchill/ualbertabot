#pragma once

#include "Common.h"
#include "BWAPI.h"
#include "GameCommander.h"
#include "UnitCommandManager.h"
#include "../../SparCraft/Source/SparCraft.h"

namespace UAlbertaBot
{
class SparCraftManager
{
	bool gameOver;
    
public:

	SparCraftManager();

	SparCraft::GameState				extractGameState();
		
	const SparCraft::IDType				getPlayerID(BWAPI::PlayerInterface * player) const;
	const bool							isCombatUnit(BWAPI::UnitInterface* unit) const;
	void                        		getMoves(SparCraft::GameState & state, std::vector<SparCraft::UnitAction> & moveVec);

	void update();
	void onStart();
	
	void doUnitMove(SparCraft::GameState & currentState, SparCraft::Unit & unit, SparCraft::UnitAction & move);
	void drawUnitMove(SparCraft::GameState & currentState, SparCraft::Unit & unit, SparCraft::UnitAction & move);
	void drawUnitCooldown(BWAPI::UnitInterface* unit);
	void drawUnitHP(BWAPI::UnitInterface* unit);
	void drawSearchResults(int x, int y);
	void drawAttackDebug();
	void performSparCraft();

    SparCraft::Unit getSparCraftUnit(const BWAPI::UnitInterface* bwapiUnit, BWAPI::GameWrapper & game);
	BWAPI::UnitInterface* getUnit(SparCraft::Unit & unit);
	const std::pair<int, int> getUnitCooldown(BWAPI::UnitInterface* unit, SparCraft::Unit & u) const;
};
}