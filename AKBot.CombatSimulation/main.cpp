#include <iostream>
#include <string>
#include <simulation\FAPCombatEstimator.h>
#include <simulation\SparCraftCombatEstimator.h>
#include <GameHelper.h>
#include <UnitHelper.h>
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
constexpr bool TOTAL_FAILURE = true;

void doTest(const std::string name, std::function<bool(CombatEstimator&)> worker, CombatEstimator& estimator, bool expected)
{
	cout << name << endl;
	auto result = worker(estimator) == expected ? "SUCCESS" : "FAIL";
	cout << "Result: " << result << endl;
}

std::shared_ptr<AKBot::GameImpl> currentGame;

void setupFakeGame()
{
	auto gameData = std::make_shared<BWAPI::GameData>();
	auto rawGameData = gameData.get();
	setP2PForces(rawGameData);
	setPlayers(*rawGameData, 2);
	currentGame = std::make_shared<AKBot::GameImpl>(rawGameData);
	auto game = currentGame.get();
	BWAPI::BroodwarPtr = game;
}

bool testZergling1(CombatEstimator& estimator)
{
	setupFakeGame();

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
	setupFakeGame();

	BWAPI::UnitData unit;
	placeTerranMarine(unit, 1, 1);
	unit.player = 0;
	AKBot::UnitImpl unitImpl(1, unit);

	std::vector<BWAPI::Unit> ourCombatUnits;
	ourCombatUnits.push_back(&unitImpl);

	std::vector<UAlbertaBot::UnitInfo> enemyCombatUnits;
	return estimator.isWinPredicted(ourCombatUnits, enemyCombatUnits, 100);
}

void testCombatSimulation(std::string name, CombatEstimator& estimator)
{
	cout << "Testing combat simulation for combat estimator " << name << endl;

	doTest("1 Zergling vs empty squad", testZergling1, estimator, TOTAL_SUCCESS);
	doTest("1 Marine vs empty squad", testMarine1, estimator, TOTAL_SUCCESS);
}

int main(int argc, char *argv[])
{
	BotMicroConfiguration configuration;
	FAPCombatEstimator fap(configuration);
	testCombatSimulation("FAP", fap);

	cout << "Press any key to continue." << endl;
	cin.get();
	return 0;
}
