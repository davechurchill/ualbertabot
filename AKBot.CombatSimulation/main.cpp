#include <iostream>
#include <iomanip>
#include <string>
#include <experimental/filesystem>
#include <simulation\FAPCombatEstimator.h>
#include <simulation\SparCraftCombatEstimator.h>
#include "BWAPIOpponentView.h"
#include <SupportLib/GameHelper.h>
#include <SupportLib/UnitHelper.h>
#include <SupportLib/NullLogger.h>
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

BWAPI::GameData rawGameData;

std::shared_ptr<AKBot::GameImpl> setupFakeGame()
{
	ZeroMemory(&rawGameData, sizeof(BWAPI::GameData));
	FillMemory(&rawGameData.getGroundHeight, sizeof(BWAPI::GameData::getGroundHeight), 0);
	FillMemory(&rawGameData.isVisible, sizeof(BWAPI::GameData::isVisible), true);
	setP2PForces(&rawGameData);
	setPlayers(rawGameData, 2);
	rawGameData.enemy = 1;

	auto currentGame = std::make_shared<AKBot::GameImpl>(&rawGameData);
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
	cout << name << " ";
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
	cout << result << endl;
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
	ZeroMemory(&unit, sizeof(unit));
	placeZergZergeling(unit, 1, 1);
	unit.player = 0;
	unit.isVisible[0] = true;
	unit.isVisible[1] = true;
	AKBot::UnitImpl unitImpl(1, unit);

	std::vector<BWAPI::Unit> ourCombatUnits;
	ourCombatUnits.push_back(&unitImpl);

	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	return estimator.isWinPredicted(ourCombatUnits, enemyCombatUnits, 100);
}

bool testMarine1(CombatEstimator& estimator)
{
	BWAPI::UnitData unit;
	ZeroMemory(&unit, sizeof(unit));
	placeTerranMarine(unit, 1, 1);
	unit.player = 0;
	unit.isVisible[0] = true;
	unit.isVisible[1] = true;
	AKBot::UnitImpl unitImpl(1, unit);

	std::vector<BWAPI::Unit> ourCombatUnits;
	ourCombatUnits.push_back(&unitImpl);

	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	return estimator.isWinPredicted(ourCombatUnits, enemyCombatUnits, 100);
}

bool testMarine2(CombatEstimator& estimator)
{
	BWAPI::UnitData marineUnitData;
	ZeroMemory(&marineUnitData, sizeof(marineUnitData));
	placeTerranMarine(marineUnitData, 1, 1);
	marineUnitData.player = 0;
	marineUnitData.isVisible[0] = true;
	marineUnitData.isVisible[1] = true;
	AKBot::UnitImpl marineUnitImpl(1, marineUnitData);

	std::vector<BWAPI::Unit> ourCombatUnits;
	ourCombatUnits.push_back(&marineUnitImpl);

	BWAPI::UnitData zerglingUnitData;
	ZeroMemory(&zerglingUnitData, sizeof(zerglingUnitData));
	placeZergZergeling(zerglingUnitData, 2, 2);
	zerglingUnitData.player = 1;
	zerglingUnitData.isVisible[0] = true;
	zerglingUnitData.isVisible[1] = true;
	AKBot::UnitImpl zerglingUnitImpl(2, zerglingUnitData);

	UAlbertaBot::UnitInfo zergling(&zerglingUnitImpl);
	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	enemyCombatUnits.push_back(zergling);
	return estimator.isWinPredicted(ourCombatUnits, enemyCombatUnits, 100);
}

bool test5MarineVs1SunkenColony(CombatEstimator& estimator)
{
	constexpr int MarinesCount = 5;
	int unitId = 1;
	int unitIndex = 0;
	BWAPI::UnitData unitData[MarinesCount];
	ZeroMemory(&unitData, sizeof(unitData));
	std::vector<shared_ptr<AKBot::UnitImpl>> units;
	for (auto i = 0; i < MarinesCount; i++)
	{
		auto& currentUnit = unitData[unitIndex];
		placeTerranMarine(currentUnit, 2, i + 1);
		currentUnit.player = 0;
		currentUnit.isVisible[0] = true;
		currentUnit.isVisible[1] = true;
		auto marineUnitImpl = std::make_shared<AKBot::UnitImpl>(unitId, currentUnit);
		units.push_back(marineUnitImpl);
		unitIndex++;
		unitId++;
	}

	std::vector<BWAPI::Unit> ourCombatUnits;
	for (auto unit : units)
	{
		ourCombatUnits.push_back(unit.get());
	}

	BWAPI::UnitData sunkenColonyUnitData;
	ZeroMemory(&sunkenColonyUnitData, sizeof(sunkenColonyUnitData));
	placeZergSunkenColony(sunkenColonyUnitData, 1, 3);
	sunkenColonyUnitData.player = 1;
	sunkenColonyUnitData.isVisible[0] = true;
	sunkenColonyUnitData.isVisible[1] = true;
	AKBot::UnitImpl sunkenColonyUnitImpl(unitId, sunkenColonyUnitData);
	unitId++;

	UAlbertaBot::UnitInfo sunkenColony(&sunkenColonyUnitImpl);
	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	enemyCombatUnits.push_back(sunkenColony);
	return estimator.isWinPredicted(ourCombatUnits, enemyCombatUnits, 100);
}

bool test8MarineVs1SunkenColony(CombatEstimator& estimator)
{
	constexpr int MarinesCount = 8;
	int unitId = 1;
	int unitIndex = 0;
	BWAPI::UnitData unitData[MarinesCount];
	ZeroMemory(&unitData, sizeof(unitData));
	std::vector<shared_ptr<AKBot::UnitImpl>> units;
	for (auto i = 0; i < MarinesCount; i++)
	{
		auto& currentUnit = unitData[unitIndex];
		placeTerranMarine(currentUnit, 2, i + 1);
		currentUnit.player = 0;
		currentUnit.isVisible[0] = true;
		currentUnit.isVisible[1] = true;
		auto marineUnitImpl = std::make_shared<AKBot::UnitImpl>(unitId, currentUnit);
		units.push_back(marineUnitImpl);
		unitIndex++;
		unitId++;
	}

	std::vector<BWAPI::Unit> ourCombatUnits;
	for (auto unit : units)
	{
		ourCombatUnits.push_back(unit.get());
	}

	BWAPI::UnitData sunkenColonyUnitData;
	ZeroMemory(&sunkenColonyUnitData, sizeof(sunkenColonyUnitData));
	placeZergSunkenColony(sunkenColonyUnitData, 1, 4);
	sunkenColonyUnitData.player = 1;
	sunkenColonyUnitData.isVisible[0] = true;
	sunkenColonyUnitData.isVisible[1] = true;
	AKBot::UnitImpl sunkenColonyUnitImpl(unitId, sunkenColonyUnitData);
	unitId++;

	UAlbertaBot::UnitInfo sunkenColony(&sunkenColonyUnitImpl);
	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	enemyCombatUnits.push_back(sunkenColony);
	return estimator.isWinPredicted(ourCombatUnits, enemyCombatUnits, 100);
}

bool test9MarineVs1SunkenColony(CombatEstimator& estimator)
{
	constexpr int MarinesCount = 9;
	int unitId = 1;
	int unitIndex = 0;
	BWAPI::UnitData unitData[MarinesCount];
	ZeroMemory(&unitData, sizeof(unitData));
	std::vector<shared_ptr<AKBot::UnitImpl>> units;
	for (auto i = 0; i < MarinesCount; i++)
	{
		placeTerranMarine(unitData[unitIndex], 2, i + 1);
		unitData[unitIndex].player = 0;
		auto marineUnitImpl = std::make_shared<AKBot::UnitImpl>(unitId, unitData[unitIndex]);
		units.push_back(marineUnitImpl);
		unitIndex++;
		unitId++;
	}

	std::vector<BWAPI::Unit> ourCombatUnits;
	for (auto unit : units)
	{
		ourCombatUnits.push_back(unit.get());
	}

	BWAPI::UnitData sunkenColonyUnitData;
	ZeroMemory(&sunkenColonyUnitData, sizeof(sunkenColonyUnitData));
	placeZergSunkenColony(sunkenColonyUnitData, 1, 5);
	sunkenColonyUnitData.player = 1;
	sunkenColonyUnitData.isVisible[0] = true;
	sunkenColonyUnitData.isVisible[1] = true;
	AKBot::UnitImpl sunkenColonyUnitImpl(unitId, sunkenColonyUnitData);
	unitId++;

	UAlbertaBot::UnitInfo sunkenColony(&sunkenColonyUnitImpl);
	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	enemyCombatUnits.push_back(sunkenColony);
	return estimator.isWinPredicted(ourCombatUnits, enemyCombatUnits, 100);
}

bool test2MarineVs1Hydralisk(CombatEstimator& estimator)
{
	constexpr int MarinesCount = 2;
	int unitId = 1;
	int unitIndex = 0;
	BWAPI::UnitData unitData[MarinesCount];
	ZeroMemory(&unitData, sizeof(unitData));
	std::vector<shared_ptr<AKBot::UnitImpl>> units;
	for (auto i = 0; i < MarinesCount; i++)
	{
		auto& currentUnit = unitData[unitIndex];
		placeTerranMarine(currentUnit, 2, i + 1);
		currentUnit.player = 0;
		currentUnit.isVisible[0] = true;
		currentUnit.isVisible[1] = true;
		auto marineUnitImpl = std::make_shared<AKBot::UnitImpl>(unitId, currentUnit);
		units.push_back(marineUnitImpl);
		unitIndex++;
		unitId++;
	}

	std::vector<BWAPI::Unit> ourCombatUnits;
	for (auto unit : units)
	{
		ourCombatUnits.push_back(unit.get());
	}

	BWAPI::UnitData hydraliskUnitData;
	ZeroMemory(&hydraliskUnitData, sizeof(hydraliskUnitData));
	placeZergHydralisk(hydraliskUnitData, 1, 2);
	hydraliskUnitData.player = 1;
	hydraliskUnitData.angle = 128;
	hydraliskUnitData.isVisible[0] = true;
	hydraliskUnitData.isVisible[1] = true;
	AKBot::UnitImpl hydraliskUnitImpl(unitId, hydraliskUnitData);
	unitId++;

	UAlbertaBot::UnitInfo hydralisk(&hydraliskUnitImpl);
	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	enemyCombatUnits.push_back(hydralisk);
	return estimator.isWinPredicted(ourCombatUnits, enemyCombatUnits, 100);
}

bool test1ZealotVs2Zealots(CombatEstimator& estimator)
{
	constexpr int MarinesCount = 1;
	constexpr int EnemyZealotsCount = 2;
	int unitId = 1;
	int unitIndex = 0;
	BWAPI::UnitData unitData[MarinesCount];
	ZeroMemory(&unitData, sizeof(unitData));
	std::vector<shared_ptr<AKBot::UnitImpl>> units;
	for (auto i = 0; i < MarinesCount; i++)
	{
		auto& currentUnit = unitData[unitIndex];
		placeProtossZealot(currentUnit, 2, i + 1);
		currentUnit.player = 0;
		currentUnit.isVisible[0] = true;
		currentUnit.isVisible[1] = true;
		//auto marineUnitImpl = std::make_shared<AKBot::UnitImpl>(unitId, currentUnit);
		units.push_back(
			std::make_shared<AKBot::UnitImpl>(unitId, currentUnit)
		);
		unitIndex++;
		unitId++;
	}

	std::vector<BWAPI::Unit> ourCombatUnits;
	for (auto unit : units)
	{
		ourCombatUnits.push_back(unit.get());
	}

	BWAPI::UnitData enemyUnitData[EnemyZealotsCount];
	ZeroMemory(&enemyUnitData, sizeof(enemyUnitData));
	
	std::vector<shared_ptr<AKBot::UnitImpl>> enemyUnits;
	unitIndex = 0;
	for (auto i = 0; i < EnemyZealotsCount; i++)
	{
		auto& currentUnit = enemyUnitData[unitIndex];
		placeProtossZealot(currentUnit, 2, i + 1);
		currentUnit.player = 1;
		currentUnit.isVisible[0] = true;
		currentUnit.isVisible[1] = true;
		//auto marineUnitImpl = std::make_shared<AKBot::UnitImpl>(unitId, currentUnit);
		enemyUnits.push_back(
			std::make_shared<AKBot::UnitImpl>(unitId, currentUnit)
		);
		unitIndex++;
		unitId++;
	}

	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	for (auto& enemyUnit : enemyUnits)
	{
		UAlbertaBot::UnitInfo enemy(enemyUnit.get());
		enemyCombatUnits.push_back(enemy);
	}

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
	doTest<TCombatEstimator>("5 Marine vs 1 Sunken Colony", test5MarineVs1SunkenColony, estimatorFactory, TOTAL_FAILURE);
	doTest<TCombatEstimator>("8 Marine vs 1 Sunken Colony", test8MarineVs1SunkenColony, estimatorFactory, TOTAL_SUCCESS);
	doTest<TCombatEstimator>("9 Marine vs 1 Sunken Colony", test9MarineVs1SunkenColony, estimatorFactory, TOTAL_SUCCESS);
	doTest<TCombatEstimator>("2 Marine vs 1 Hydralisk", test2MarineVs1Hydralisk, estimatorFactory, TOTAL_SUCCESS);
	doTest<TCombatEstimator>("1 Zealot vs 2 Zealots", test1ZealotVs2Zealots, estimatorFactory, TOTAL_FAILURE);

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
	auto sparcraftConfigFile = path.concat("/SparCraft_Config.txt").string();
	SparCraft::AIParameters::Instance().parseFile(sparcraftConfigFile);
	return SparCraftCombatEstimator(
		opponentView,
		logger,
		sparcraftConfiguration,
		configuration);
}

int main(int argc, char *argv[])
{
	configuration.CombatEstimationDepth = 2000;
	testCombatSimulation<FAPCombatEstimator>("FAP", getFap);

	testCombatSimulation<SparCraftCombatEstimator>("SparCraft", getSparCraft);

	cout << "Press any key to continue." << endl;
	cin.get();
	return 0;
}
