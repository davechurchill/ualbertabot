#include "ProductionManagerDebug.h"

namespace AKBot
{
	using UAlbertaBot::CompareWhenStarted;

	ProductionManagerDebug::ProductionManagerDebug(shared_ptr<ProductionManager> productionManager)
		: _productionManager(productionManager)
	{
	}

	void ProductionManagerDebug::drawProductionInformation(AKBot::ScreenCanvas& canvas, int x, int y) const
	{
		auto buildingManager = _productionManager->getBuildingManager();
		drawBuildingInformation(canvas, buildingManager, 200, 50);
		auto bossManager = _productionManager->getBOSSManager();
		drawSearchInformation(canvas, bossManager, 490, 100);
		drawStateInformation(canvas, 250, 0, buildingManager);

		if (!Config.Debug.DrawProductionInfo)
		{
			return;
		}

		// fill prod with each unit which is under construction
		std::vector<BWAPI::Unit> prod;
		const auto& opponentView = _productionManager->getOpponentView();
		for (auto & unit : opponentView->self()->getUnits())
		{
			UAB_ASSERT(unit != nullptr, "Unit was null");

			if (unit->isBeingConstructed())
			{
				prod.push_back(unit);
			}
		}

		// sort it based on the time it was started
		std::sort(prod.begin(), prod.end(), CompareWhenStarted());

		canvas.drawTextScreen(x - 30, y + 20, "\x04 TIME");
		canvas.drawTextScreen(x, y + 20, "\x04 UNIT NAME");

		y += 30;
		int yy = y;

		// for each unit in the _queue
		for (auto & unit : prod)
		{
			std::string prefix = "\x07";

			yy += 10;

			BWAPI::UnitType t = unit->getType();
			if (t == BWAPI::UnitTypes::Zerg_Egg)
			{
				t = unit->getBuildType();
			}

			canvas.drawTextScreen(x, yy, " %s%s", prefix.c_str(), t.getName().c_str());
			canvas.drawTextScreen(x - 35, yy, "%s%6d", prefix.c_str(), unit->getRemainingBuildTime());
		}

		drawQueueInformation(canvas, _productionManager->getBuildOrderQueue(), x, yy + 10);
	}

	void ProductionManagerDebug::drawQueueInformation(AKBot::ScreenCanvas& canvas, const BuildOrderQueue& buildQueue, int x, int y) const
	{
		//x = x + 25;

		if (!Config.Debug.DrawProductionInfo)
		{
			return;
		}

		std::string prefix = "\x04";

		size_t reps = buildQueue.size() < 12 ? buildQueue.size() : 12;

		// for each unit in the queue
		for (size_t i = 0; i<reps; i++) {

			prefix = "\x04";

			const auto & type = buildQueue[buildQueue.size() - 1 - i].metaType;

			if (type.isUnit())
			{
				if (type.getUnitType().isWorker())
				{
					prefix = "\x1F";
				}
				else if (type.getUnitType().supplyProvided() > 0)
				{
					prefix = "\x03";
				}
				else if (type.getUnitType().isRefinery())
				{
					prefix = "\x1E";
				}
				else if (type.isBuilding())
				{
					prefix = "\x11";
				}
				else if (type.getUnitType().groundWeapon() != BWAPI::WeaponTypes::None || type.getUnitType().airWeapon() != BWAPI::WeaponTypes::None)
				{
					prefix = "\x06";
				}
			}

			canvas.drawTextScreen(x, y + (i * 10), " %s%s", prefix.c_str(), type.getName().c_str());
		}
	}

	void ProductionManagerDebug::drawBuildingInformation(AKBot::ScreenCanvas& canvas, shared_ptr<BuildingManager> buildingManager, int x, int y) const
	{
		drawReservedTiles(canvas, buildingManager->getBuildingPlacer());

		if (!Config.Debug.DrawBuildingInfo)
		{
			return;
		}

		const auto& opponentView = _productionManager->getOpponentView();
		for (auto & unit : opponentView->self()->getUnits())
		{
			canvas.drawTextMap(unit->getPosition().x, unit->getPosition().y + 5, "\x07%d", unit->getID());
		}

		canvas.drawTextScreen(x, y, "\x04 Building Information:");
		canvas.drawTextScreen(x, y + 20, "\x04 Name");
		canvas.drawTextScreen(x + 150, y + 20, "\x04 State");

		int yspace = 0;

		for (const auto & b : buildingManager->getBuildings())
		{
			auto workerCode = buildingManager->getBuildingWorkerCode(b);
			if (b.status == UAlbertaBot::BuildingStatus::Unassigned)
			{
				canvas.drawTextScreen(x, y + 40 + ((yspace) * 10), "\x03 %s", b.type.getName().c_str());
				canvas.drawTextScreen(x + 150, y + 40 + ((yspace++) * 10), "\x03 Need %c", workerCode);
			}
			else if (b.status == UAlbertaBot::BuildingStatus::Assigned)
			{
				canvas.drawTextScreen(x, y + 40 + ((yspace) * 10), "\x03 %s %d", b.type.getName().c_str(), b.builderUnit->getID());
				canvas.drawTextScreen(x + 150, y + 40 + ((yspace++) * 10), "\x03 A %c (%d,%d)", workerCode, b.finalPosition.x, b.finalPosition.y);

				int x1 = b.finalPosition.x * 32;
				int y1 = b.finalPosition.y * 32;
				int x2 = (b.finalPosition.x + b.type.tileWidth()) * 32;
				int y2 = (b.finalPosition.y + b.type.tileHeight()) * 32;

				canvas.drawLineMap(b.builderUnit->getPosition().x, b.builderUnit->getPosition().y, (x1 + x2) / 2, (y1 + y2) / 2, BWAPI::Colors::Orange);
				canvas.drawBoxMap(x1, y1, x2, y2, BWAPI::Colors::Red, false);
			}
			else if (b.status == UAlbertaBot::BuildingStatus::UnderConstruction)
			{
				canvas.drawTextScreen(x, y + 40 + ((yspace) * 10), "\x03 %s %d", b.type.getName().c_str(), b.buildingUnit->getID());
				canvas.drawTextScreen(x + 150, y + 40 + ((yspace++) * 10), "\x03 Const %c", workerCode);
			}
		}
	}


	void ProductionManagerDebug::drawSearchInformation(AKBot::ScreenCanvas& canvas, shared_ptr<BOSSManager> bossManager, int x, int y) const
	{
		if (!Config.Debug.DrawBuildOrderSearchInfo)
		{
			return;
		}

		// draw the background
		int width = 155;
		int height = 80;
		canvas.drawBoxScreen(BWAPI::Position(x - 5, y), BWAPI::Position(x + width, y + height), BWAPI::Colors::Black, true);

		x += 5; y += 3;

		canvas.drawTextScreen(BWAPI::Position(x, y), "%cBuildOrderSearch:", '\x04');
		y += 10;
		canvas.drawTextScreen(BWAPI::Position(x, y), "%s", bossManager->getPreviousStatus().c_str());

		auto& previousGoalUnits = bossManager->getPreviousGoalUnits();
		for (size_t i = 0; i < previousGoalUnits.size(); ++i)
		{
			if (previousGoalUnits[i].second > 0)
			{
				y += 10;
				canvas.drawTextScreen(BWAPI::Position(x, y), "%d %s", previousGoalUnits[i].second, previousGoalUnits[i].first.getName().c_str());
			}
		}

		canvas.drawTextScreen(BWAPI::Position(x, y + 25), "Time (ms): %.3lf", bossManager->getTotalPreviousSearchTime());
		const auto& savedSearchResults = bossManager->getSavedSearchResults();
		canvas.drawTextScreen(BWAPI::Position(x, y + 35), "Nodes: %d", savedSearchResults.nodesExpanded);
		canvas.drawTextScreen(BWAPI::Position(x, y + 45), "BO Size: %d", (int)savedSearchResults.buildOrder.size());
	}

	void ProductionManagerDebug::drawStateInformation(AKBot::ScreenCanvas& canvas, int x, int y, shared_ptr<BuildingManager> buildingManager) const
	{
		if (!Config.Debug.DrawBOSSStateInfo)
		{
			return;
		}

		const auto& opponentView = _productionManager->getOpponentView();
		BOSS::GameState currentState(BWAPI::Broodwar, opponentView->self(), buildingManager->buildingsQueued());
		canvas.drawTextScreen(BWAPI::Position(x - 100, y + 30), "\x04%s", currentState.getBuildingData().toString().c_str());
		canvas.drawTextScreen(BWAPI::Position(x + 150, y), "\x04%s", currentState.toString().c_str());
	}

	void ProductionManagerDebug::drawReservedTiles(AKBot::ScreenCanvas& canvas, const BuildingPlacer& buildingPlacer) const
	{
		if (!Config.Debug.DrawReservedBuildingTiles)
		{
			return;
		}

		int rwidth = buildingPlacer.reserveWidth();
		int rheight = buildingPlacer.reserveHeight();

		for (int x = 0; x < rwidth; ++x)
		{
			for (int y = 0; y < rheight; ++y)
			{
				if (buildingPlacer.isReserved(x, y) || buildingPlacer.isInResourceBox(x, y))
				{
					int x1 = x * 32 + 8;
					int y1 = y * 32 + 8;
					int x2 = (x + 1) * 32 - 8;
					int y2 = (y + 1) * 32 - 8;

					canvas.drawBoxMap(x1, y1, x2, y2, BWAPI::Colors::Yellow, false);
				}
			}
		}
	}
}