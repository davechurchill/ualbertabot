#include "MapToolsDebug.h"
#include <algorithm>
#include <BWAPI/Position.h>

namespace AKBot
{
	MapToolsDebug::MapToolsDebug(
		shared_ptr<OpponentView> opponentView,
		shared_ptr<MapTools> map,
		shared_ptr<BaseLocationManager> bases,
		shared_ptr<HUDInfo> hudInfo,
		const BotDebugConfiguration& debugConfiguration)
		: _map(map), _bases(bases), _debugConfiguration(debugConfiguration), _opponentView(opponentView)
		, _hudInfo(hudInfo)
	{
	}

	void MapToolsDebug::draw(AKBot::ScreenCanvas& canvas, int currentFrame)
	{
		if (!_debugConfiguration.DrawLastSeenTileInfo)
		{
			return;
		}

		bool rMouseState = _hudInfo->getMouseState(BWAPI::MouseButton::M_RIGHT);
		if (!rMouseState)
		{
			return;
		}

		// draw the least recently seen position tile as a yellow circle on the map
		BWAPI::Position lrsp = _bases->getLeastRecentlySeenPosition(_map);
		canvas.drawCircleMap(lrsp, 32, BWAPI::Colors::Yellow, true);

		int size = 4;
		BWAPI::Position homePosition(_opponentView->self()->getStartLocation());
		BWAPI::Position mPos = _hudInfo->getMousePosition() + _hudInfo->getScreenPosition();
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
				BWAPI::TilePosition testLocation(x, y);
				bool isOccupied = _map->isOccupiedTile(testLocation);
				bool isBuildable = _map->isBuildableTile(testLocation)
					&& _map->isVisibleTile(testLocation)
					&& isOccupied;
				if (!isBuildable)
				{
					boxColor = BWAPI::Colors::Red;
				}

				canvas.drawBoxMap(pos, pos + diag, boxColor, false);
				canvas.drawTextMap(pos + BWAPI::Position(2, 2), "%d", currentFrame - _map->getLastSeen(x, y));
				canvas.drawTextMap(pos + BWAPI::Position(2, 12), "%d", homeDist);
			}
		}

	}
}