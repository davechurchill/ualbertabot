#include <iostream>
#include <iomanip>
#include <string>
#include <experimental/filesystem>
#include <simulation\FAPCombatEstimator.h>
#include <simulation\SparCraftCombatEstimator.h>
#include "BWAPIOpponentView.h"
#include <GameHelper.h>
#include <UnitHelper.h>
#include <NullLogger.h>
#include <BWAPI/Client/GameData.h>
#include <BWAPI/Client/UnitData.h>
#include <TestLib/GameImpl.h>
#include <TestLib/UnitImpl.h>

using namespace AKBot;
using namespace AKBot::Tests;
using namespace UAlbertaBot;
using std::cout;
using std::cin;
using std::endl;

constexpr bool TOTAL_SUCCESS = true;
constexpr bool TOTAL_FAILURE = false;

int successTests = 0;
int totalTests = 0;

BotMicroConfiguration configuration;
BotSparCraftConfiguration sparcraftConfiguration;

std::shared_ptr<AKBot::GameImpl> setupFakeGame()
{
	auto gameData = std::make_shared<BWAPI::GameData>();
	auto rawGameData = gameData.get();
	setP2PForces(rawGameData);
	setPlayers(*rawGameData, 2);
	rawGameData->enemy = 1;

	auto currentGame = std::make_shared<AKBot::GameImpl>(rawGameData);
	currentGame->onMatchStart();
	return currentGame;
}

template<typename TCombatEstimator>
void doTest(
	const std::string name,
	std::function<bool(CombatEstimator&)> worker,
	std::function<TCombatEstimator(std::shared_ptr<AKBot::GameImpl>)> estimatorFactory,
	bool expected)
{
	cout << name << endl;
	auto currentGame = setupFakeGame();
	auto game = currentGame.get();
	BWAPI::BroodwarPtr = game;
	auto estimator = estimatorFactory(currentGame);
	auto success = worker(estimator) == expected;
	BWAPI::BroodwarPtr = nullptr;
	if (success)
	{
		successTests++;
	}
	
	totalTests++;
	auto result = success ? "SUCCESS" : "FAIL";
	cout << "Result: " << result << endl;
}

void clearTestCounter()
{
	successTests = 0;
	totalTests = 0;
}
void printStatistics()
{
	cout << std::fixed;
	cout.precision(2);

	cout << "Statistics: " << successTests << " out of " << totalTests << " tests passed" << endl;
	auto successRate = 100.0 * successTests / totalTests;
	cout << "Success rate: " << successRate << "%" << endl;
}

bool testZergling1(CombatEstimator& estimator)
{
	BWAPI::UnitData unit;
	placeZergZergeling(unit, 1, 1);
	unit.player = 0;
	AKBot::UnitImpl unitImpl(1, unit);

	std::vector<BWAPI::Unit> ourCombatUnits;
	ourCombatUnits.push_back(&unitImpl);

	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	return estimator.isWinPredicted(ourCombatUnits, enemyCombatUnits, 100);
}

bool testMarine1(CombatEstimator& estimator)
{
	BWAPI::UnitData unit;
	placeTerranMarine(unit, 1, 1);
	unit.player = 0;
	AKBot::UnitImpl unitImpl(1, unit);

	std::vector<BWAPI::Unit> ourCombatUnits;
	ourCombatUnits.push_back(&unitImpl);

	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	return estimator.isWinPredicted(ourCombatUnits, enemyCombatUnits, 100);
}

bool testMarine2(CombatEstimator& estimator)
{
	BWAPI::UnitData marineUnitData;
	placeTerranMarine(marineUnitData, 1, 1);
	marineUnitData.player = 0;
	AKBot::UnitImpl marineUnitImpl(1, marineUnitData);

	std::vector<BWAPI::Unit> ourCombatUnits;
	ourCombatUnits.push_back(&marineUnitImpl);

	BWAPI::UnitData zerglingUnitData;
	placeZergZergeling(zerglingUnitData, 2, 2);
	zerglingUnitData.player = 1;
	AKBot::UnitImpl zerglingUnitImpl(2, zerglingUnitData);

	UAlbertaBot::UnitInfo zergling(&zerglingUnitImpl);
	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	enemyCombatUnits.push_back(zergling);
	return estimator.isWinPredicted(ourCombatUnits, enemyCombatUnits, 100);
}

template<typename TCombatEstimator>
void testCombatSimulation(
	std::string name,
	std::function<TCombatEstimator(std::shared_ptr<AKBot::GameImpl>)> estimatorFactory)
{
	clearTestCounter();
	cout << "Testing combat simulation for combat estimator " << name << endl;

	doTest<TCombatEstimator>("1 Zergling vs empty squad", testZergling1, estimatorFactory, TOTAL_SUCCESS);
	doTest<TCombatEstimator>("1 Marine vs empty squad", testMarine1, estimatorFactory, TOTAL_SUCCESS);
	doTest<TCombatEstimator>("1 Marine vs 1 Zergling", testMarine2, estimatorFactory, TOTAL_FAILURE);

	printStatistics();
}

FAPCombatEstimator getFap(std::shared_ptr<AKBot::GameImpl> gameImpl)
{
	return FAPCombatEstimator(configuration);
}

SparCraftCombatEstimator getSparCraft(std::shared_ptr<AKBot::GameImpl> gameImpl)
{
	auto game = gameImpl.get();
	auto opponentView = std::make_shared<BWAPIOpponentView>(game);
	auto logger = std::make_shared<AkBotTests::NullLogger>();
	SparCraft::init();
	auto path = std::experimental::filesystem::current_path();
	auto sparcraftConfigFile = path.concat("/../../../../UAlbertaBot/bin/SparCraft_Config.txt").string();
	SparCraft::AIParameters::Instance().parseFile(sparcraftConfigFile);
	return SparCraftCombatEstimator(
		opponentView,
		logger,
		sparcraftConfiguration,
		configuration);
}

int main(int argc, char *argv[])
{
	testCombatSimulation<FAPCombatEstimator>("FAP", getFap);

	testCombatSimulation<SparCraftCombatEstimator>("SparCraft", getSparCraft);

	cout << "Press any key to continue." << endl;
	cin.get();
	return 0;
}
