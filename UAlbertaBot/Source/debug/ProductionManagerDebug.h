#pragma once

#include "..\ProductionManager.h"
#include "..\ScreenCanvas.h"
#include "..\BotConfiguration.h"

namespace AKBot
{
	using UAlbertaBot::ProductionManager;
	using UAlbertaBot::BuildingManager;
	using UAlbertaBot::BuildOrderQueue;
	using UAlbertaBot::BOSSManager;
	using UAlbertaBot::BuildingPlacer;

	class ProductionManagerDebug
	{
		shared_ptr<ProductionManager> _productionManager;
		shared_ptr<AKBot::Logger> _logger;
		const BotDebugConfiguration& _debugConfiguration;

		void drawBuildingInformation(ScreenCanvas& canvas, shared_ptr<BuildingManager> buildingManager, int x, int y) const;
		void drawQueueInformation(AKBot::ScreenCanvas& canvas, const BuildOrderQueue& buildQueue, int x, int y) const;
		void drawStateInformation(AKBot::ScreenCanvas& canvas, int x, int y, shared_ptr<BuildingManager> buildingManager) const;
		void drawSearchInformation(AKBot::ScreenCanvas& canvas, shared_ptr<BOSSManager> bossManager, int x, int y) const;
		void drawReservedTiles(AKBot::ScreenCanvas& canvas, const BuildingPlacer& buildingPlacer) const;
	public:
		ProductionManagerDebug(
			shared_ptr<ProductionManager> productionManager,
			shared_ptr<AKBot::Logger> logger,
			const BotDebugConfiguration& debugConfiguration);
		void drawProductionInformation(AKBot::ScreenCanvas& canvas, int x, int y) const;
	};
}