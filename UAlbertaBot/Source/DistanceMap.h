#pragma once

#include <vector>
#include <functional>
#include <BWAPI/Position.h>

namespace UAlbertaBot
{

	typedef std::function<bool(BWAPI::TilePosition)> TileCheckFunc;
    
class DistanceMap
{
    int _width;
    int _height;
    BWAPI::TilePosition _startTile;
	TileCheckFunc _isWalkable;

    std::vector<std::vector<int>> _dist;
    std::vector<BWAPI::TilePosition> _sortedTilePositions;

    void computeDistanceMap(const BWAPI::TilePosition & startTile, TileCheckFunc isWalkable);

public:

    DistanceMap();
    DistanceMap(const BWAPI::TilePosition & startTile, int width, int height, TileCheckFunc isWalkable);

    const int & getDistance(const int & tileX, const int & tileY) const;
    const int & getDistance(const BWAPI::Position & pos) const;
    const int & getDistance(const BWAPI::TilePosition & pos) const;

    // given a position, get the position we should move to to minimize distance
    const std::vector<BWAPI::TilePosition> & getSortedTiles() const;
};
}