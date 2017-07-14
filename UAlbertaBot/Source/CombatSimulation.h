#pragma once

#include "Common.h"

#ifdef USING_VISUALIZATION_LIBRARIES
	#include "Visualizer.h"
#endif

#include "..\..\SparCraft\source\GameState.h"
#include "..\..\SparCraft\source\Game.h"
#include "..\..\SparCraft\source\Unit.h"
#include "..\..\SparCraft\source\AllPlayers.h"
#include "UnitData.h"
#include "OpponentView.h"
#include "UnitInfoManager.h"

namespace UAlbertaBot
{
class CombatSimulation
{
	SparCraft::GameState		_state;
	const AKBot::OpponentView& _opponentView;
	const UnitInfoManager& _unitInfo;

public:

	CombatSimulation(const AKBot::OpponentView& opponentView, const UnitInfoManager& unitInfo);

	void setCombatUnits(const BWAPI::Position & center, const int radius);

	double simulateCombat();

	const SparCraft::Unit			getSparCraftUnit(const UnitInfo & ui) const;
    const SparCraft::Unit			getSparCraftUnit(BWAPI::Unit unit) const;
	const SparCraft::GameState &	getSparCraftState() const;

	const size_t getSparCraftPlayerID(BWAPI::Player player) const;

	void logState(const SparCraft::GameState & state);
};
}