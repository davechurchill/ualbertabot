#include "FAPCombatEstimator.h"
#include "..\UnitUtil.h"

using namespace AKBot;
using namespace UAlbertaBot;

double AKBot::defaultScoreFunction(const AKBot::FAPCombatEstimator& estimator)
{
	auto scores = estimator.playerScores();

	auto score = scores.first - scores.second;
	return double(score);
}

double AKBot::deltaScoreFunction(const AKBot::FAPCombatEstimator& estimator)
{
	auto initialScores = estimator.getInitialScores();
	auto scores = estimator.playerScores();

	auto firstPlayerDamage = initialScores.first - scores.first;
	auto secondPlayerDamage = initialScores.second - scores.second;
	auto score = firstPlayerDamage - secondPlayerDamage;

	// Reverse to indicate that positive score is where we have less
	// damage then enemy.
	return double(1 - score);
}

double AKBot::unitHealthScore(const FAPPlayerState& state)
{
	double result = 0.0;
	for (auto & u : state) {
		if (u.health && u.maxHealth) {
			result += (u.score * u.health) / (u.maxHealth * 2);
		}
	}

	return result;
}

double AKBot::economicalCostScore(const FAPPlayerState& state)
{
	double result = 0.0;
	for (auto & u : state) {
		if (u.health && u.maxHealth) {
			result += u.unitType.mineralPrice() + u.unitType.gasPrice();
		}
	}

	return result;
}

AKBot::FAPCombatEstimator::FAPCombatEstimator(const BotMicroConfiguration & microConfiguration)
	: _microConfiguration(microConfiguration)
	, _scoreFunction(AKBot::defaultScoreFunction)
	, _playerScoreFunction(unitHealthScore)
{
}

AKBot::FAPCombatEstimator::FAPCombatEstimator(
	std::function<double(const FAPPlayerState&)> playerScoreFunction,
	std::function<double(const FAPCombatEstimator&)> scoreFunction,
	const BotMicroConfiguration & microConfiguration)
	: _microConfiguration(microConfiguration)
	, _scoreFunction(scoreFunction)
	, _playerScoreFunction(playerScoreFunction)
{
}

std::pair<double, double> AKBot::FAPCombatEstimator::calculateScore(std::function<double(const FAPPlayerState&)> playerScoreFunction)
{
	auto state = getState();
	return std::make_pair(
		playerScoreFunction(*state.first),
		playerScoreFunction(*state.second));
}

bool FAPCombatEstimator::isWinPredicted(
	const std::vector<BWAPI::Unit>& ourCombatUnits,
	const std::vector<UAlbertaBot::UnitInfo>& enemyCombatUnits,
	int currentFrame)
{
	setCombatUnits(ourCombatUnits, enemyCombatUnits, currentFrame);
	auto score = simulateCombat();
	return score > _microConfiguration.RetreatThreshold;
}


// sets the starting states based on the combat units within a radius of a given position
// this center will most likely be the position of the forwardmost combat unit we control
void FAPCombatEstimator::setCombatUnits(
	const std::vector<BWAPI::Unit> ourCombatUnits,
	std::vector<UnitInfo> enemyCombatUnits,
	int currentFrame)
{
	std::ignore = currentFrame;
	for (auto & unit : ourCombatUnits)
	{
		if (unit->getType().isWorker() || unit->getHitPoints() == 0 || unit->getType().isBuilding())
		{
			continue;
		}

		if (UnitUtil::IsCombatUnit(unit))
		{
			fap.addIfCombatUnitPlayer1(unit);
		}
	}

	for (UnitInfo & ui : enemyCombatUnits)
	{
		if (ui.type.isWorker() || ui.lastHealth == 0 || ui.type == BWAPI::UnitTypes::Unknown)
		{
			continue;
		}

		if (!ui.type.isFlyer() && ui.completed)
		{
			fap.addIfCombatUnitPlayer2(ui);
		}
	}
}

double AKBot::FAPCombatEstimator::simulateCombat()
{
	initialScores = calculateScore(_playerScoreFunction);

	fap.simulate(_microConfiguration.CombatEstimationDepth);
	finalScores = calculateScore(_playerScoreFunction);
	return _scoreFunction(*this);
}