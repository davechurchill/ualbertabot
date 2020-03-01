#pragma once

#include "Common.h"

#ifdef USING_VISUALIZATION_LIBRARIES
	#include "Visualizer.h"
#endif

#include "..\..\SparCraft\source\GameState.h"
#include "..\..\SparCraft\source\Game.h"
#include "..\..\SparCraft\source\Unit.h"
#include "..\..\SparCraft\source\AllPlayers.h"

namespace UAlbertaBot
{
struct UnitInfo;
class CombatSimulation
{
	SparCraft::GameState m_state;

public:

	CombatSimulation();

	void setCombatUnits(const BWAPI::Position & center, const int radius);

	SparCraft::ScoreType            simulateCombat();
	const SparCraft::Unit			getSparCraftUnit(const UnitInfo & ui) const;
    const SparCraft::Unit			getSparCraftUnit(BWAPI::Unit unit) const;
	const SparCraft::GameState &	getSparCraftState() const;
	const size_t                    getSparCraftPlayerID(BWAPI::Player player) const;
};
}