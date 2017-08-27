#include "WorkerManagerDebug.h"

namespace AKBot
{
	WorkerManagerDebug::WorkerManagerDebug(shared_ptr<WorkerData> workerData)
		: _workerData(workerData)
	{
	}
	void WorkerManagerDebug::draw(ScreenCanvas & canvas)
	{
		if (Config.Debug.DrawResourceInfo)
		{
			drawResourceDebugInfo(canvas);
		}

		if (Config.Debug.DrawWorkerInfo)
		{
			drawWorkerInformation(canvas, 450, 20);
		}

		drawDepotDebugInfo(canvas);
	}

	void WorkerManagerDebug::drawResourceDebugInfo(AKBot::ScreenCanvas& canvas)
	{
		for (auto & worker : _workerData->getWorkers())
		{
			UAB_ASSERT(worker != nullptr, "Worker was null");

			char job = _workerData->getJobCode(worker);

			BWAPI::Position pos = worker->getTargetPosition();

			canvas.drawTextMap(worker->getPosition().x, worker->getPosition().y - 5, "\x07%c", job);
			canvas.drawLineMap(worker->getPosition().x, worker->getPosition().y, pos.x, pos.y, BWAPI::Colors::Cyan);

			BWAPI::Unit depot = _workerData->getWorkerDepot(worker);
			if (depot)
			{
				canvas.drawLineMap(worker->getPosition().x, worker->getPosition().y, depot->getPosition().x, depot->getPosition().y, BWAPI::Colors::Orange);
			}
		}
	}

	void WorkerManagerDebug::drawWorkerInformation(AKBot::ScreenCanvas& canvas, int x, int y)
	{
		canvas.drawTextScreen(x, y, "\x04 Workers %d", _workerData->getNumMineralWorkers());
		canvas.drawTextScreen(x, y + 20, "\x04 UnitID");
		canvas.drawTextScreen(x + 50, y + 20, "\x04 State");

		int yspace = 0;

		for (auto & unit : _workerData->getWorkers())
		{
			UAB_ASSERT(unit != nullptr, "Worker was null");

			canvas.drawTextScreen(x, y + 40 + ((yspace) * 10), "\x03 %d", unit->getID());
			canvas.drawTextScreen(x + 50, y + 40 + ((yspace++) * 10), "\x03 %c", _workerData->getJobCode(unit));
		}
	}

	void WorkerManagerDebug::drawDepotDebugInfo(AKBot::ScreenCanvas& canvas) const
	{
		for (auto & depot : _workerData->getDepots())
		{
			int x = depot->getPosition().x - 64;
			int y = depot->getPosition().y - 32;

			if (Config.Debug.DrawWorkerInfo)
			{
				canvas.drawBoxMap(x - 2, y - 1, x + 75, y + 14, BWAPI::Colors::Black, true);
				canvas.drawTextMap(x, y, "\x04 Workers: %d", _workerData->getNumAssignedWorkers(depot));
			}

			std::vector<BWAPI::Unit> minerals = _workerData->getMineralPatchesNearDepot(depot);
			auto workersOnMineralPatch = _workerData->getWorkersOnMineralPatch();
			for (auto & mineral : minerals)
			{
				if (workersOnMineralPatch.find(mineral) != workersOnMineralPatch.end())
				{
					auto mineralPosition = mineral->getPosition();
					int mineralX = mineralPosition.x;
					int mineralY = mineralPosition.y;

					canvas.drawBoxMap(mineralX - 2, mineralY - 1, mineralX + 75, mineralY + 14, BWAPI::Colors::Black, true);
					canvas.drawTextMap(mineralX, mineralY, "\x04 Workers: %d", workersOnMineralPatch[mineral]);
				}
			}
		}
	}
}