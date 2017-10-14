#include "MapToolsDebug.h"
#include <algorithm>
#include <BWAPI.h>

namespace AKBot
{
	MapToolsDebug::MapToolsDebug(
		shared_ptr<MapTools> map,
		shared_ptr<BaseLocationManager> bases,
		const BotDebugConfiguration& debugConfiguration)
		: _map(map), _bases(bases), _debugConfiguration(debugConfiguration)
	{
	}

	void MapToolsDebug::draw(AKBot::ScreenCanvas& canvas)
	{
		if (!_debugConfiguration.DrawLastSeenTileInfo)
		{
			return;
		}

		bool rMouseState = BWAPI::Broodwar->getMouseState(BWAPI::MouseButton::M_RIGHT);
		if (!rMouseState)
		{
			return;
		}

		// draw the least recently seen position tile as a yellow circle on the map
		BWAPI::Position lrsp = _bases->getLeastRecentlySeenPosition(_map);
		canvas.drawCircleMap(lrsp, 32, BWAPI::Colors::Yellow, true);


		int size = 4;
		BWAPI::Position homePosition(BWAPI::Broodwar->self()->getStartLocation());
		BWAPI::Position mPos = BWAPI::Broodwar->getMousePosition() + BWAPI::Broodwar->getScreenPosition();
		BWAPI::TilePosition mTile(mPos);

		int xStart = std::max(mTile.x - size, 0);
		int xEnd = std::min(mTile.x + size, (int)_map->getWidth());
		int yStart = std::max(mTile.y - size, 0);
		int yEnd = std::min(mTile.y + size, (int)_map->getHeight());

		for (int x = xStart; x < xEnd; ++x)
		{
			for (int y = yStart; y < yEnd; ++y)
			{
				BWAPI::Position pos(x * 32, y * 32);
				BWAPI::Position diag(32, 32);
				int homeDist = _map->getGroundDistance(pos, homePosition);

				BWAPI::Color boxColor = BWAPI::Colors::Green;
				if (!BWAPI::Broodwar->isBuildable(BWAPI::TilePosition(x, y), true))
				{
					boxColor = BWAPI::Colors::Red;
				}

				canvas.drawBoxMap(pos, pos + diag, boxColor, false);
				canvas.drawTextMap(pos + BWAPI::Position(2, 2), "%d", BWAPI::Broodwar->getFrameCount() - _map->getLastSeen(x, y));
				canvas.drawTextMap(pos + BWAPI::Position(2, 12), "%d", homeDist);
			}
		}

	}
}