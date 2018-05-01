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
	UnitBuilder zerglingBuilder(unit);
	zerglingBuilder
		.unit(UnitTypes::Zerg_Zergling)
		.position(1, 1)
		.visibleToAll();
	AKBot::UnitImpl unitImpl(1, unit);

	std::vector<BWAPI::Unit> ourCombatUnits;
	ourCombatUnits.push_back(&unitImpl);

	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	return estimator.isWinPredicted(ourCombatUnits, enemyCombatUnits, 100);
}

bool testMarine1(CombatEstimator& estimator)
{
	BWAPI::UnitData marineUnitData;
	UnitBuilder marineBuilder(marineUnitData);
	marineBuilder
		.unit(UnitTypes::Terran_Marine)
		.position(1, 1)
		.visibleToAll();
	AKBot::UnitImpl unitImpl(1, marineUnitData);

	std::vector<BWAPI::Unit> ourCombatUnits;
	ourCombatUnits.push_back(&unitImpl);

	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	return estimator.isWinPredicted(ourCombatUnits, enemyCombatUnits, 100);
}

bool test1MarineVs1Zergling(CombatEstimator& estimator)
{
	BWAPI::UnitData marineUnitData;
	UnitBuilder marineBuilder(marineUnitData);
	marineBuilder
		.unit(UnitTypes::Terran_Marine)
		.position(1, 1)
		.visibleToAll();
	AKBot::UnitImpl marineUnitImpl(1, marineUnitData);

	std::vector<BWAPI::Unit> ourCombatUnits;
	ourCombatUnits.push_back(&marineUnitImpl);

	BWAPI::UnitData zerglingUnitData;
	UnitBuilder zerglingBuilder(zerglingUnitData);
	zerglingBuilder
		.unit(UnitTypes::Zerg_Zergling)
		.position(2, 2)
		.player(1)
		.visibleToAll();
	AKBot::UnitImpl zerglingUnitImpl(2, zerglingUnitData);

	UAlbertaBot::UnitInfo zergling(&zerglingUnitImpl);
	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	enemyCombatUnits.push_back(zergling);
	return estimator.isWinPredicted(ourCombatUnits, enemyCombatUnits, 100);
}

bool test1MarineVs1Broodling(CombatEstimator& estimator)
{
	BWAPI::UnitData marineUnitData;
	UnitBuilder marineBuilder(marineUnitData);
	marineBuilder
		.unit(UnitTypes::Terran_Marine)
		.position(1, 1)
		.visibleToAll();
	AKBot::UnitImpl marineUnitImpl(1, marineUnitData);

	std::vector<BWAPI::Unit> ourCombatUnits;
	ourCombatUnits.push_back(&marineUnitImpl);

	BWAPI::UnitData broodlingUnitData;
	UnitBuilder broodlingBuilder(broodlingUnitData);
	broodlingBuilder
		.unit(UnitTypes::Zerg_Broodling)
		.position(2, 2)
		.player(1)
		.visibleToAll();
	AKBot::UnitImpl broodlingUnitImpl(2, broodlingUnitData);

	UAlbertaBot::UnitInfo broodling(&broodlingUnitImpl);
	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	enemyCombatUnits.push_back(broodling);
	return estimator.isWinPredicted(ourCombatUnits, enemyCombatUnits, 100);
}

bool test1BroodlingVs1Marine(CombatEstimator& estimator)
{
	BWAPI::UnitData broodlingUnitData;
	UnitBuilder broodlingBuilder(broodlingUnitData);
	broodlingBuilder
		.unit(UnitTypes::Zerg_Broodling)
		.position(TilePosition(1, 1))
		.my()
		.visibleToAll();
	AKBot::UnitImpl broodlingUnitImpl(1, broodlingUnitData);

	std::vector<BWAPI::Unit> ourCombatUnits;
	ourCombatUnits.push_back(&broodlingUnitImpl);

	BWAPI::UnitData marineUnitData;
	UnitBuilder marineBuilder(marineUnitData);
	marineBuilder
		.unit(UnitTypes::Terran_Marine)
		.position(TilePosition(2, 2))
		.player(1)
		.visibleToAll();
	AKBot::UnitImpl marineUnitImpl(2, marineUnitData);

	UAlbertaBot::UnitInfo marine(&marineUnitImpl);
	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	enemyCombatUnits.push_back(marine);
	return estimator.isWinPredicted(ourCombatUnits, enemyCombatUnits, 100);
}

bool test2BroodlingVs1Marine(CombatEstimator& estimator)
{
	BWAPI::UnitData broodlingUnitData;
	UnitBuilder broodlingBuilder(broodlingUnitData);
	broodlingBuilder
		.unit(UnitTypes::Zerg_Broodling)
		.position(TilePosition(1, 1))
		.my()
		.visibleToAll();
	AKBot::UnitImpl broodlingUnitImpl(1, broodlingUnitData);

	BWAPI::UnitData broodling2UnitData;
	UnitBuilder broodling2Builder(broodling2UnitData);
	broodling2Builder
		.unit(UnitTypes::Zerg_Broodling)
		.position(TilePosition(1, 2))
		.my()
		.visibleToAll();
	AKBot::UnitImpl broodling2UnitImpl(2, broodling2UnitData);

	std::vector<BWAPI::Unit> ourCombatUnits;
	ourCombatUnits.push_back(&broodlingUnitImpl);
	ourCombatUnits.push_back(&broodling2UnitImpl);

	BWAPI::UnitData marineUnitData;
	UnitBuilder marineBuilder(marineUnitData);
	marineBuilder
		.unit(UnitTypes::Terran_Marine)
		.position(TilePosition(2, 2))
		.player(1)
		.visibleToAll();
	AKBot::UnitImpl marineUnitImpl(3, marineUnitData);

	UAlbertaBot::UnitInfo marine(&marineUnitImpl);
	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	enemyCombatUnits.push_back(marine);
	return estimator.isWinPredicted(ourCombatUnits, enemyCombatUnits, 100);
}

bool test2BroodlingNearRemovalVs1Marine(CombatEstimator& estimator)
{
	BWAPI::UnitData broodlingUnitData;
	UnitBuilder broodlingBuilder(broodlingUnitData);
	broodlingBuilder
		.unit(UnitTypes::Zerg_Broodling)
		.position(TilePosition(1, 1))
		.my()
		.visibleToAll();
	broodlingBuilder.data().removeTimer = 50;
	AKBot::UnitImpl broodlingUnitImpl(1, broodlingUnitData);

	BWAPI::UnitData broodling2UnitData;
	UnitBuilder broodling2Builder(broodling2UnitData);
	broodling2Builder
		.unit(UnitTypes::Zerg_Broodling)
		.position(TilePosition(1, 2))
		.my()
		.visibleToAll();
	broodling2Builder.data().removeTimer = 50;
	AKBot::UnitImpl broodling2UnitImpl(2, broodling2UnitData);

	std::vector<BWAPI::Unit> ourCombatUnits;
	ourCombatUnits.push_back(&broodlingUnitImpl);
	ourCombatUnits.push_back(&broodling2UnitImpl);

	BWAPI::UnitData marineUnitData;
	UnitBuilder marineBuilder(marineUnitData);
	marineBuilder
		.unit(UnitTypes::Terran_Marine)
		.position(TilePosition(2, 2))
		.player(1)
		.visibleToAll();
	AKBot::UnitImpl marineUnitImpl(3, marineUnitData);

	UAlbertaBot::UnitInfo marine(&marineUnitImpl);
	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	enemyCombatUnits.push_back(marine);
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
		UnitBuilder currentBuilder(currentUnit);
		currentBuilder
			.unit(UnitTypes::Terran_Marine)
			.position(2, i+1)
			.visibleToAll();
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
	UnitBuilder sunken(sunkenColonyUnitData);
	sunken
		.unit(UnitTypes::Zerg_Sunken_Colony)
		.position(1, 3)
		.player(1)
		.visibleToAll();
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
		UnitBuilder currentBuilder(currentUnit);
		currentBuilder
			.unit(UnitTypes::Terran_Marine)
			.position(2, i + 1)
			.visibleToAll();
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
	UnitBuilder sunken(sunkenColonyUnitData);
	sunken
		.unit(UnitTypes::Zerg_Sunken_Colony)
		.position(1, 3)
		.player(1)
		.visibleToAll();
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
		auto& currentUnit = unitData[unitIndex];
		UnitBuilder currentBuilder(currentUnit);
		currentBuilder
			.unit(UnitTypes::Terran_Marine)
			.position(2, i + 1)
			.visibleToAll();
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
	UnitBuilder sunken(sunkenColonyUnitData);
	sunken
		.unit(UnitTypes::Zerg_Sunken_Colony)
		.position(1, 3)
		.player(1)
		.visibleToAll();
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
		UnitBuilder currentBuilder(currentUnit);
		currentBuilder
			.unit(UnitTypes::Terran_Marine)
			.position(2, i + 1)
			.visibleToAll();
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
	UnitBuilder hydraliskBuilder(hydraliskUnitData);
	hydraliskBuilder
		.unit(UnitTypes::Zerg_Hydralisk)
		.position(1, 2)
		.player(1)
		.angle(128)
		.visibleToAll();
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
		UnitBuilder currentBuilder(currentUnit);
		currentBuilder
			.unit(UnitTypes::Protoss_Zealot)
			.position(2, i + 1)
			.visibleToAll();
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
		UnitBuilder currentBuilder(currentUnit);
		currentBuilder
			.unit(UnitTypes::Protoss_Zealot)
			.position(2, i + 1)
			.visibleToAll();
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
	doTest<TCombatEstimator>("1 Marine vs 1 Zergling", test1MarineVs1Zergling, estimatorFactory, TOTAL_FAILURE);
	doTest<TCombatEstimator>("5 Marine vs 1 Sunken Colony", test5MarineVs1SunkenColony, estimatorFactory, TOTAL_FAILURE);
	doTest<TCombatEstimator>("8 Marine vs 1 Sunken Colony", test8MarineVs1SunkenColony, estimatorFactory, TOTAL_SUCCESS);
	doTest<TCombatEstimator>("9 Marine vs 1 Sunken Colony", test9MarineVs1SunkenColony, estimatorFactory, TOTAL_SUCCESS);
	doTest<TCombatEstimator>("2 Marine vs 1 Hydralisk", test2MarineVs1Hydralisk, estimatorFactory, TOTAL_SUCCESS);
	doTest<TCombatEstimator>("1 Zealot vs 2 Zealots", test1ZealotVs2Zealots, estimatorFactory, TOTAL_FAILURE);
	doTest<TCombatEstimator>("1 Marine vs 1 Broodling", test1MarineVs1Broodling, estimatorFactory, TOTAL_SUCCESS);
	doTest<TCombatEstimator>("1 Broodling vs 1 Marine", test1BroodlingVs1Marine, estimatorFactory, TOTAL_FAILURE);
	doTest<TCombatEstimator>("2 Broodling vs 1 Marine", test2BroodlingVs1Marine, estimatorFactory, TOTAL_SUCCESS);
	doTest<TCombatEstimator>("2 Broodling near removal vs 1 Marine", test2BroodlingNearRemovalVs1Marine, estimatorFactory, TOTAL_FAILURE);

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
