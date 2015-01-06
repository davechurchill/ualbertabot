#pragma once

#include "Common.h"
#include "MapGrid.h"

#ifdef USING_VISUALIZATION_LIBRARIES
	#include "Visualizer.h"
#endif

#include "..\..\SparCraft\source\GameState.h"
#include "..\..\SparCraft\source\Game.h"
#include "..\..\SparCraft\source\Unit.h"
#include "..\..\SparCraft\source\AllPlayers.h"
#include "InformationManager.h"

namespace UAlbertaBot
{
class CombatSimulation
{

	SparCraft::GameState		state;
	bool						hasLogged;

public:

	CombatSimulation();

	void setCombatUnits(const BWAPI::Position & center, const int radius);
    bool checkZealotVsZergling(const BWAPI::Position & center, const int radius);

	SparCraft::ScoreType simulateCombat();

	const SparCraft::Unit			getSparCraftUnit(const UnitInfo & ui) const;
    const SparCraft::Unit			getSparCraftUnit(BWAPI::UnitInterface* unit) const;
	const SparCraft::GameState &	getSparCraftState() const;

	const SparCraft::IDType getSparCraftPlayerID(BWAPI::PlayerInterface * player) const;

	void logState(const SparCraft::GameState & state);
};
}