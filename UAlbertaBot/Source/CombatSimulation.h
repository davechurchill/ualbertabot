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
#include "Logger.h"

namespace UAlbertaBot
{
class CombatSimulation
{
	SparCraft::GameState		_state;
	const AKBot::OpponentView& _opponentView;
	const UnitInfoManager& _unitInfo;
	const AKBot::Logger& _logger;

public:

	CombatSimulation(const AKBot::OpponentView& opponentView, const UnitInfoManager& unitInfo, const AKBot::Logger& logger);

	void setCombatUnits(const BWAPI::Position & center, const int radius, int currentFrame);

	double simulateCombat();

	const SparCraft::Unit			getSparCraftUnit(const UnitInfo & ui, int currentFrame) const;
    const SparCraft::Unit			getSparCraftUnit(BWAPI::Unit unit, int currentFrame) const;
	const SparCraft::GameState &	getSparCraftState() const;

	const size_t getSparCraftPlayerID(BWAPI::Player player) const;

	void logState(const SparCraft::GameState & state);
};
}