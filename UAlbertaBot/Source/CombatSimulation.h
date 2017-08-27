#pragma once

#include "Common.h"

#ifdef USING_VISUALIZATION_LIBRARIES
	#include "Visualizer.h"
#endif

#pragma warning( push )
#pragma warning( disable : 4100)
#include "..\..\SparCraft\source\GameState.h"
#include "..\..\SparCraft\source\Game.h"
#include "..\..\SparCraft\source\Unit.h"
#include "..\..\SparCraft\source\AllPlayers.h"
#pragma warning( pop )
#include "UnitData.h"
#include "OpponentView.h"
#include "UnitInfoManager.h"
#include "Logger.h"

namespace UAlbertaBot
{
class CombatSimulation
{
	SparCraft::GameState		_state;
	SparCraft::GameState		_evaluatedState;
	double						_lastScore;
	shared_ptr<AKBot::OpponentView> _opponentView;
	std::shared_ptr<AKBot::Logger> _logger;

public:

	CombatSimulation(
		shared_ptr<AKBot::OpponentView> opponentView,
		shared_ptr<AKBot::Logger> logger);

	void setCombatUnits(
		const std::vector<BWAPI::Unit> ourCombatUnits,
		std::vector<UnitInfo> enemyCombatUnits,
		const BWAPI::Position & center,
		const int radius,
		int currentFrame);

	double simulateCombat();

	const SparCraft::Unit			getSparCraftUnit(const UnitInfo & ui, int currentFrame) const;
    const SparCraft::Unit			getSparCraftUnit(BWAPI::Unit unit, int currentFrame) const;
	const SparCraft::GameState &	getSparCraftState() const;

	const size_t getSparCraftPlayerID(BWAPI::Player player) const;
	const double getLastScore() const { return _lastScore; }
	const SparCraft::GameState& getEvaluatedState() { return _evaluatedState; }
};
}