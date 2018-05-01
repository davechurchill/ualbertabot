#include "BotFactory.h"
#include "UAlbertaBot_Arena.h"
#include "UAlbertaBot_Tournament.h"
#include "BWAPIOpponentView.h"
#include "BWAPIReplayOpponentView.h"
#include "BWAPIMapInformation.h"

#include "debug\BWAPIPrintLogger.h"
#include "debug\BaseLocationManagerDebug.h"
#include "debug\GameCommanderDebug.h"
#include "debug\UnitInfoManagerDebug.h"
#include "debug\WorkerManagerDebug.h"
#include "debug\MapToolsDebug.h"
#include "debug\DebugInfoProvider.h"
#include "BWAPIHUDInfo.h"
#include "BWAPIUnitInformation.h"
#include "strategies\protoss\ZealotRush.h"
#include "strategies\protoss\ZealotDrop.h"
#include "strategies\protoss\DragoonRush.h"
#include "strategies\protoss\DarkTemplarRush.h"
#include "strategies\terrain\MarineRush.h"
#include "strategies\terrain\FourBarracksMarine.h"
#include "strategies\terrain\TankPush.h"
#include "strategies\terrain\VultureRush.h"
#include "strategies\zerg\ZergelingRush.h"
#include "strategies\zerg\TwoHatchHydralisk.h"
#include "strategies\zerg\ThreeHatchMutalisk.h"
#include "strategies\zerg\ThreeHatchScourge.h"
#include "basedetection\ClusterBaseDetector.h"

#include "ParseUtils.h"

using namespace AKBot;
using namespace UAlbertaBot;

void registerWellKnownStrategies(AKBot::OpponentView& opponentView, StrategyManager& strategyManager) {
	auto self = opponentView.self();

	// Well known Protoss strategies.
	strategyManager.registerStrategy("Protoss_ZealotRush", std::make_unique<ZealotRush>(self));
	strategyManager.registerStrategy("Protoss_Drop", std::make_unique<ZealotDrop>(self));
	strategyManager.registerStrategy("Protoss_DragoonRush", std::make_unique<DragoonRush>(self));
	strategyManager.registerStrategy("Protoss_DTRush", std::make_unique<DarkTemplarRush>(self));

	// Well known Terrain strategies
	strategyManager.registerStrategy("Terran_MarineRush", std::make_unique<MarineRush>(self));
	strategyManager.registerStrategy("Terran_4RaxMarines", std::make_unique<FourBarracksMarine>(self));
	strategyManager.registerStrategy("Terran_TankPush", std::make_unique<TankPush>(self));
	strategyManager.registerStrategy("Terran_VultureRush", std::make_unique<VultureRush>(self));

	// Well known Zerg strategies
	strategyManager.registerStrategy("Zerg_ZerglingRush", std::make_unique<ZergelingRush>(self));
	strategyManager.registerStrategy("Zerg_9Pool", std::make_unique<ZergelingRush>(self));
	strategyManager.registerStrategy("Zerg_2HatchHydra", std::make_unique<TwoHatchHydralisk>(self));
	strategyManager.registerStrategy("Zerg_3HatchMuta", std::make_unique<ThreeHatchMutalisk>(self));
	strategyManager.registerStrategy("Zerg_3HatchScourge", std::make_unique<ThreeHatchScourge>(self));
}

BotPlayer AKBot::createBot(BWAPI::Game* game, const std::string& mode, BotConfiguration& configuration, const std::string& configurationFile) {
	if (mode == "Tournament") {
		auto logger = std::shared_ptr<AKBot::Logger>(new AKBot::BWAPIPrintLogger());
		auto opponentView = game->isReplay()
			? std::shared_ptr<AKBot::OpponentView>(new AKBot::BWAPIReplayOpponentView(game))
			: std::shared_ptr<AKBot::OpponentView>(new AKBot::BWAPIOpponentView(game));
		auto unitInformation = std::shared_ptr<AKBot::UnitInformation>(new AKBot::BWAPIUnitInformation(game));
		auto workerData = std::shared_ptr<WorkerData>(new WorkerData(logger, unitInformation));
		auto workerManager = std::shared_ptr<WorkerManager>(new WorkerManager(
			opponentView,
			workerData));
		auto autoObserver = std::shared_ptr<AutoObserver>(new AutoObserver(opponentView, workerManager));
		auto mapInformation = std::shared_ptr<MapInformation>(new BWAPIMapInformation(game));
		auto mapTools = std::shared_ptr<MapTools>(new MapTools(mapInformation, logger));
		auto baseLocationManager = std::shared_ptr<BaseLocationManager>(new BaseLocationManager(
			game,
			opponentView,
			configuration.BaseDetection));
		baseLocationManager->registerBaseDetector(
			"uab",
			std::make_unique<AKBot::ClusterBaseDetector>(
				game,
				opponentView,
				mapTools));
		auto unitInfoManager = std::shared_ptr<UnitInfoManager>(new UnitInfoManager(opponentView));
		auto strategyManager = std::shared_ptr<StrategyManager>(new StrategyManager(
			opponentView,
			unitInfoManager,
			baseLocationManager,
			logger,
			configuration.Strategy));
		registerWellKnownStrategies(*opponentView, *strategyManager);
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
			unitInformation,
			logger,
			configuration.Macro));
		auto productionManager = std::shared_ptr<ProductionManager>(new ProductionManager(
			opponentView,
			bossManager,
			buildingManager,
			strategyManager,
			workerManager,
			unitInfoManager,
			unitInformation,
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
		auto hubInfo = std::shared_ptr<HUDInfo>(new BWAPIHUDInfo(game));
		std::vector<shared_ptr<DebugInfoProvider>> providers = {
			std::shared_ptr<DebugInfoProvider>(new AKBot::GameCommanderDebug(gameCommander, logger, hubInfo, debugConfiguration, configuration.Strategy)),
			std::shared_ptr<DebugInfoProvider>(new AKBot::BaseLocationManagerDebug(opponentView, baseLocationManager, mapTools, configuration.Debug)),
			std::shared_ptr<DebugInfoProvider>(new AKBot::UnitInfoManagerDebug(opponentView, unitInfoManager, unitInformation, debugConfiguration)),
			std::shared_ptr<DebugInfoProvider>(new AKBot::WorkerManagerDebug(workerData, debugConfiguration)),
			std::shared_ptr<DebugInfoProvider>(new AKBot::MapToolsDebug(opponentView, mapTools, baseLocationManager, hubInfo, debugConfiguration)),
		};
		shared_ptr<GameDebug> gameDebug = std::shared_ptr<GameDebug>(new GameDebug(providers));

		ParseUtils::ParseStrategy(opponentView, configuration.Strategy, strategyManager);
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
