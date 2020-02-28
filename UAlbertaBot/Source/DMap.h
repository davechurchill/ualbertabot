#pragma once

#include "Common.h"
#include <map>

namespace UAlbertaBot
{

	class DMap
	{
		int m_width;
		int m_height;
		BWAPI::TilePosition m_startTile;

		// 2D matrix storing distances from the start tile
		std::vector<std::vector<int>> m_dist;

		std::vector<BWAPI::TilePosition> m_sortedTiles;

	public:

		DMap();
		void computeDistanceMap(const BWAPI::TilePosition & startTile);

		int getDistance(int tileX, int tileY) const;
		int getDistance(const BWAPI::TilePosition & pos) const;
		int getDistance(const BWAPI::Position & pos) const;

		// given a position, get the position we should move to to minimize distance
		const std::vector<BWAPI::TilePosition> & getSortedTiles() const;
		const BWAPI::TilePosition & getStartTile() const;

		void draw() const;
	};

}