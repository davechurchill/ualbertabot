#pragma once
#include <memory>

#pragma warning( push )
#pragma warning( disable : 4100)
#include "..\..\..\SparCraft\source\GameState.h"
#include "..\..\..\SparCraft\source\Game.h"
#include "..\..\..\SparCraft\source\Unit.h"
#include "..\..\..\SparCraft\source\AllPlayers.h"
#include "..\..\..\SparCraft\source\AIParameters.h"
#pragma warning( pop )

#include "CombatEstimator.h"
#include "..\BotConfiguration.h"
#include "..\OpponentView.h"
#include "..\Logger.h"

namespace AKBot
{
	class SparCraftCombatEstimator : public CombatEstimator
	{
		SparCraft::GameState		_state;
		SparCraft::GameState		_evaluatedState;
		double						_lastScore;
		
		std::shared_ptr<AKBot::OpponentView> _opponentView;
		std::shared_ptr<AKBot::Logger> _logger;
		const BotSparCraftConfiguration& _sparcraftConfiguration;
		const BotMicroConfiguration& _microConfiguration;

		void setCombatUnits(
			const std::vector<BWAPI::Unit> ourCombatUnits,
			std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits,
			int currentFrame);

		double simulateCombat();

		const SparCraft::Unit			getSparCraftUnit(const UAlbertaBot::UnitInfo & ui, int currentFrame) const;
		const SparCraft::Unit			getSparCraftUnit(BWAPI::Unit unit, int currentFrame) const;

		const size_t getSparCraftPlayerID(BWAPI::Player player) const;
	public:
		SparCraftCombatEstimator(
			shared_ptr<AKBot::OpponentView> opponentView,
			shared_ptr<AKBot::Logger> logger,
			const BotSparCraftConfiguration& sparcraftConfiguration,
			const BotMicroConfiguration& microConfiguration);
		bool isWinPredicted(
			const std::vector<BWAPI::Unit> ourCombatUnits,
			std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits,
			int currentFrame) override;

		const SparCraft::GameState &	getSparCraftState() const;
		const double getLastScore() const { return _lastScore; }
		const SparCraft::GameState& getEvaluatedState() { return _evaluatedState; }
	};
}