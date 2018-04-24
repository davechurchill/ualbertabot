#pragma once
#include "CombatEstimator.h"
#include "FAP.h"
#include "..\BotConfiguration.h"

namespace AKBot
{
	typedef std::vector<UAlbertaBot::FastAPproximation::FAPUnit> FAPPlayerState;

	class FAPCombatEstimator : public CombatEstimator
	{
	private:
		UAlbertaBot::FastAPproximation fap;
		std::pair<double, double> initialScores;
		std::pair<double, double> finalScores;
		std::function<double(const FAPCombatEstimator&)> _scoreFunction;
		std::function<double(const FAPPlayerState&)> _playerScoreFunction;
		
		const BotMicroConfiguration& _microConfiguration;

		void setCombatUnits(
			const std::vector<BWAPI::Unit> ourCombatUnits,
			std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits,
			int currentFrame);
		double simulateCombat();
		std::pair<double, double> calculateScore(std::function<double(const FAPPlayerState&)> playerScoreFunction);
	public:
		FAPCombatEstimator(
			const BotMicroConfiguration& microConfiguration);
		FAPCombatEstimator(
			std::function<double(const FAPPlayerState&)> playerScoreFunction,
			std::function<double(const FAPCombatEstimator&)> scoreFunction,
			const BotMicroConfiguration& microConfiguration);
		bool isWinPredicted(
			const std::vector<BWAPI::Unit>& ourCombatUnits,
			const std::vector<UAlbertaBot::UnitInfo>& enemyCombatUnits,
			int currentFrame) override;

		std::pair<FAPPlayerState*, FAPPlayerState*> getState() {
			return fap.getState();
		}
		std::pair<double, double> playerScores() const { return finalScores; }
		std::pair<int, int> playerScoresUnits() const { return fap.playerScoresUnits(); }
		std::pair<int, int> playerScoresBuildings() const { return fap.playerScoresBuildings(); }

		std::pair<double, double> getInitialScores() const { return initialScores; }
	};
	
	double defaultScoreFunction(const FAPCombatEstimator& estimator);
	double deltaScoreFunction(const FAPCombatEstimator& estimator);
	double unitHealthScore(const FAPPlayerState& state);
	double economicalCostScore(const FAPPlayerState& state);
}