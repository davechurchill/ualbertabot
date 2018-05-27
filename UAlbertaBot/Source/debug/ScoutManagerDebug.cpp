#include "ScoutManagerDebug.h"

namespace AKBot
{
	ScoutManagerDebug::ScoutManagerDebug(shared_ptr<ScoutManager> scoutManager)
		: _scoutManager(scoutManager)
	{
	}

	void ScoutManagerDebug::drawScoutInformation(AKBot::ScreenCanvas& canvas, int x, int y) const
	{
		canvas.drawTextScreen(x, y, "ScoutInfo: %s", _scoutManager->getScoutStatus().c_str());
		auto& enemyRegionVerticies = _scoutManager->getEnemyRegionVerticies();
		for (size_t i(0); i < enemyRegionVerticies.size(); ++i)
		{
			canvas.drawCircleMap(enemyRegionVerticies[i], 4, BWAPI::Colors::Green, false);
			canvas.drawTextMap(enemyRegionVerticies[i], "%d", i);
		}

		auto& workerScout = _scoutManager->getWorkerScount();
		if (workerScout)
		{
			if (workerScout->isCarryingGas())
			{
				canvas.drawCircleMap(workerScout->getPosition(), 10, BWAPI::Colors::Purple, true);
			}
		}
		else
		{
			canvas.drawTextScreen(x, y + 12, "Worker scout missing");
		}
	}
}