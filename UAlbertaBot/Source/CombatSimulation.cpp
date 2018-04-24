#include "CombatSimulation.h"
#include "UnitUtil.h"
#include <string>
using namespace std;
using namespace UAlbertaBot;

class NullDebugInfoProvider : public AKBot::CombatEstimatorDebugProvider {
public:
	void printDebugInformation(BWAPI::Position simulationCenter) override {}
};

class SparCraftCombatEstimatorDebugProvider : public AKBot::CombatEstimatorDebugProvider {
	AKBot::SparCraftCombatEstimator &_estimator;
public:
	SparCraftCombatEstimatorDebugProvider(AKBot::SparCraftCombatEstimator &estimator): _estimator(estimator)
	{
	};
	void printDebugInformation(BWAPI::Position simulationCenter) override
	{
		BWAPI::Broodwar->drawCircleMap(simulationCenter.x, simulationCenter.y, 10, BWAPI::Colors::Red, true);
		std::stringstream ss1;
		ss1 << "Initial State:\n";
		ss1 << SparCraft::AITools::StateToStringCompact(_estimator.getSparCraftState()) << "\n\n";

		std::stringstream ss2;

		ss2 << "Predicted Outcome: " << _estimator.getLastScore() << "\n";
		ss2 << SparCraft::AITools::StateToStringCompact(_estimator.getEvaluatedState()) << "\n";

		BWAPI::Broodwar->drawTextScreen(150, 200, "%s", ss1.str().c_str());
		BWAPI::Broodwar->drawTextScreen(300, 200, "%s", ss2.str().c_str());

		BWAPI::Broodwar->drawTextScreen(240, 280, "Combat Sim : %lf", _estimator.getLastScore());
	}
};

class FAPCombatEstimatorDebugProvider : public AKBot::CombatEstimatorDebugProvider {
	AKBot::FAPCombatEstimator &_estimator;
public:
	FAPCombatEstimatorDebugProvider(AKBot::FAPCombatEstimator &estimator) : _estimator(estimator)
	{
	};
	void printDebugInformation(BWAPI::Position simulationCenter) override
	{
		BWAPI::Broodwar->drawCircleMap(simulationCenter.x, simulationCenter.y, 10, BWAPI::Colors::Red, true);
		std::stringstream ss1;
		ss1 << "Units score:\n";
		auto unitsScore = _estimator.playerScoresUnits();
		ss1 << "Me: " << unitsScore.first << " Enemy: " << unitsScore.second << "\n\n";

		std::stringstream ss2;
		ss2 << "Buildings score:\n";
		auto buildingsScore = _estimator.playerScoresBuildings();
		ss2 << "Me: " << buildingsScore.first << " Enemy: " << buildingsScore.second << "\n\n";

		std::stringstream ss3;
		ss3 << "Total score:\n";
		auto totalScore = _estimator.playerScores();
		ss3 << "Me: " << totalScore.first << " Enemy: " << totalScore.second << "\n\n";

		BWAPI::Broodwar->drawTextScreen(150, 200, "%s", ss1.str().c_str());
		BWAPI::Broodwar->drawTextScreen(300, 200, "%s", ss2.str().c_str());
		BWAPI::Broodwar->drawTextScreen(450, 200, "%s", ss3.str().c_str());
	}
};

AKBot::CombatEstimator & UAlbertaBot::CombatSimulation::getCurrentCombatEstimator()
{
	auto& entry = getCurrentSimulationEntry();
	return *entry.estimator.get();
}

const AKBot::CombatSimulationEntry & UAlbertaBot::CombatSimulation::getCurrentSimulationEntry()
{
	auto element = registry.find(_microConfiguration.CombatEstimationStrategy);
	if (element == registry.cend())
	{
		element = registry.find("sparcraft");
	}

	return element->second;
}

CombatSimulation::CombatSimulation(
	shared_ptr<AKBot::OpponentView> opponentView,
	std::shared_ptr<AKBot::Logger> logger,
	const BotSparCraftConfiguration& sparcraftConfiguration,
	const BotMicroConfiguration& microConfiguration)
	: _microConfiguration(microConfiguration)
{
	AKBot::CombatSimulationEntry sparcraftEntry;
	sparcraftEntry.estimator = make_unique<AKBot::SparCraftCombatEstimator>(opponentView, logger, sparcraftConfiguration, microConfiguration);
	sparcraftEntry.debugProvider = make_unique<SparCraftCombatEstimatorDebugProvider>(
		reinterpret_cast<AKBot::SparCraftCombatEstimator&>(*sparcraftEntry.estimator.get()));
	registry.insert(std::make_pair("sparcraft", std::move(sparcraftEntry)));

	registerFAPEstimator("fap", AKBot::unitHealthScore, AKBot::defaultScoreFunction);
	registerFAPEstimator("fapDelta", AKBot::unitHealthScore, AKBot::deltaScoreFunction);
	registerFAPEstimator("fapEconomy", AKBot::economicalCostScore, AKBot::defaultScoreFunction);
	registerFAPEstimator("fapEconomyDelta", AKBot::economicalCostScore, AKBot::deltaScoreFunction);
}

void CombatSimulation::registerFAPEstimator(
	const char* name,
	std::function<double(const AKBot::FAPPlayerState&)> playerScoreFunction,
	std::function<double(const AKBot::FAPCombatEstimator&)> scoreFunction)
{
	AKBot::CombatSimulationEntry fapEntry;
	fapEntry.estimator = make_unique<AKBot::FAPCombatEstimator>(
		playerScoreFunction,
		scoreFunction,
		_microConfiguration);
	fapEntry.debugProvider = make_unique<FAPCombatEstimatorDebugProvider>(
		reinterpret_cast<AKBot::FAPCombatEstimator&>(*fapEntry.estimator.get()));
	registry.insert(std::make_pair(name, std::move(fapEntry)));
}

// sets the starting states based on the combat units within a radius of a given position
// this center will most likely be the position of the forwardmost combat unit we control
bool CombatSimulation::isWinPredicted(
	const std::vector<BWAPI::Unit> ourCombatUnits,
	std::vector<UnitInfo> enemyCombatUnits,
	int currentFrame)
{
	return getCurrentCombatEstimator().isWinPredicted(ourCombatUnits, enemyCombatUnits, currentFrame);
}

void CombatSimulation::printDebugInformation(BWAPI::Position simulationCenter)
{
	auto& entry = getCurrentSimulationEntry();
	entry.debugProvider->printDebugInformation(simulationCenter);
	auto& estimator = getCurrentCombatEstimator();
}