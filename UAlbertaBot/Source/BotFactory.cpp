#include "BotFactory.h"
#include "UAlbertaBot_Arena.h"
#include "UAlbertaBot_Tournament.h"
#include "BWAPIOpponentView.h"
#include "BWAPIMapInformation.h"

#include "debug\BWAPIPrintLogger.h"
#include "debug\BaseLocationManagerDebug.h"
#include "debug\GameCommanderDebug.h"
#include "debug\UnitInfoManagerDebug.h"
#include "debug\WorkerManagerDebug.h"
#include "debug\MapToolsDebug.h"
#include "debug\DebugInfoProvider.h"

#include "ParseUtils.h"

using namespace AKBot;
using namespace UAlbertaBot;

BotPlayer AKBot::createBot(const std::string& mode, BotConfiguration& configuration, const std::string& configurationFile) {
	if (mode == "Tournament") {
		auto game = BWAPI::BroodwarPtr;
		auto logger = std::shared_ptr<AKBot::Logger>(new AKBot::BWAPIPrintLogger());
		auto opponentView = std::shared_ptr<AKBot::OpponentView>(new AKBot::BWAPIOpponentView(game));
		auto workerData = std::shared_ptr<WorkerData>(new WorkerData(logger));
		auto workerManager = std::shared_ptr<WorkerManager>(new WorkerManager(
			opponentView,
			workerData));
		auto autoObserver = std::shared_ptr<AutoObserver>(new AutoObserver(opponentView, workerManager));
		auto baseLocationManager = std::shared_ptr<BaseLocationManager>(new BaseLocationManager(game, opponentView));
		auto unitInfoManager = std::shared_ptr<UnitInfoManager>(new UnitInfoManager(opponentView));
		auto strategyManager = std::shared_ptr<StrategyManager>(new StrategyManager(
			opponentView,
			unitInfoManager,
			baseLocationManager,
			logger,
			configuration.Strategy));
		auto mapInformation = std::shared_ptr<MapInformation>(new BWAPIMapInformation(game));
		auto mapTools = std::shared_ptr<MapTools>(new MapTools(mapInformation, logger));
		auto combatCommander = std::shared_ptr<CombatCommander>(new CombatCommander(
			baseLocationManager,
			opponentView,
			workerManager,
			unitInfoManager,
			mapTools,
			logger,
			configuration.Micro,
			configuration.SparCraft,
			configuration.Debug));
		auto bossManager = std::shared_ptr<BOSSManager>(new BOSSManager(opponentView, configuration.Debug));
		auto scoutManager = std::shared_ptr<ScoutManager>(new ScoutManager(
			opponentView,
			baseLocationManager,
			mapTools));
		auto buildingManager = std::shared_ptr<BuildingManager>(new BuildingManager(
			opponentView,
			baseLocationManager,
			workerManager,
			mapTools,
			logger,
			configuration.Macro));
		auto productionManager = std::shared_ptr<ProductionManager>(new ProductionManager(
			opponentView,
			bossManager,
			buildingManager,
			strategyManager,
			workerManager,
			unitInfoManager,
			baseLocationManager,
			mapTools,
			configuration.Debug));
		auto gameCommander = std::shared_ptr<GameCommander>(new GameCommander(
			opponentView,
			bossManager,
			combatCommander,
			scoutManager,
			productionManager,
			workerManager
		));
		auto& debugConfiguration = configuration.Debug;
		std::vector<shared_ptr<DebugInfoProvider>> providers = {
			std::shared_ptr<DebugInfoProvider>(new AKBot::GameCommanderDebug(gameCommander, logger, debugConfiguration, configuration.Strategy)),
			std::shared_ptr<DebugInfoProvider>(new AKBot::BaseLocationManagerDebug(opponentView, baseLocationManager, mapTools, configuration.Debug)),
			std::shared_ptr<DebugInfoProvider>(new AKBot::UnitInfoManagerDebug(opponentView, unitInfoManager, debugConfiguration)),
			std::shared_ptr<DebugInfoProvider>(new AKBot::WorkerManagerDebug(workerData, debugConfiguration)),
			std::shared_ptr<DebugInfoProvider>(new AKBot::MapToolsDebug(mapTools, baseLocationManager, debugConfiguration)),
		};
		shared_ptr<GameDebug> gameDebug = std::shared_ptr<GameDebug>(new GameDebug(providers));

		ParseUtils::ParseStrategy(configuration.Strategy, strategyManager);
		auto strategyName = configuration.Strategy.StrategyName;
		strategyManager->setPreferredStrategy(strategyName);

		scoutManager->setScoutHarassEnemy(configuration.Strategy.ScoutHarassEnemy);
		combatCommander->setSupportsDropSquad(strategyName == "Protoss_Drop");
		if (strategyName == "Protoss_DTRush")
		{
			combatCommander->setRushModeEnabled(true);
			combatCommander->setRushUnitType(BWAPI::UnitTypes::Protoss_Dark_Templar);
			combatCommander->setAllowedRushUnitLoses(0);
		}

		productionManager->setUseBuildOrderSearch(configuration.Modules.UsingBuildOrderSearch);
		workerManager->setWorkersPerRefinery(configuration.Macro.WorkersPerRefinery);

		return BotPlayer(std::shared_ptr<BotModule>(new UAlbertaBot_Tournament(
			configuration,
			baseLocationManager,
			autoObserver,
			strategyManager,
			unitInfoManager,
			mapTools,
			combatCommander,
			gameCommander,
			gameDebug)));
	}
	else if (mode == "Arena") {
		return BotPlayer(std::shared_ptr<BotModule>(new UAlbertaBot_Arena(configuration)));
	}
	else {
		return BotPlayer(std::shared_ptr<BotModule>());
	}
}
