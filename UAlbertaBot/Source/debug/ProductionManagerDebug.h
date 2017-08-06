#pragma once

#include "ProductionManager.h"
#include "ScreenCanvas.h"

namespace AKBot
{
	using UAlbertaBot::ProductionManager;
	using UAlbertaBot::BuildingManager;
	using UAlbertaBot::BuildOrderQueue;
	using UAlbertaBot::BOSSManager;
	using UAlbertaBot::BuildingPlacer;

	class ProductionManagerDebug
	{
		const ProductionManager& _productionManager;
		void drawBuildingInformation(ScreenCanvas& canvas, const BuildingManager& buildingManager, int x, int y) const;
		void drawQueueInformation(AKBot::ScreenCanvas& canvas, const BuildOrderQueue& buildQueue, int x, int y) const;
		void drawStateInformation(AKBot::ScreenCanvas& canvas, int x, int y, const BuildingManager & buildingManager) const;
		void drawSearchInformation(AKBot::ScreenCanvas& canvas, const BOSSManager& bossManager, int x, int y) const;
		void drawReservedTiles(AKBot::ScreenCanvas& canvas, const BuildingPlacer& buildingPlacer) const;
	public:
		ProductionManagerDebug(const ProductionManager& productionManager);
		void drawProductionInformation(AKBot::ScreenCanvas& canvas, int x, int y) const;
	};
}