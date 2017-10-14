#include "DistanceMap.h"
#include "UABAssert.h"
#include "Timer.hpp"
#include <functional>

using namespace UAlbertaBot;

const size_t LegalActions = 4;
const int actionX[LegalActions] = {1, -1, 0, 0};
const int actionY[LegalActions] = {0, 0, 1, -1};

DistanceMap::DistanceMap()
{

}

DistanceMap::DistanceMap(const BWAPI::TilePosition & startTile, int width, int height, TileCheckFunc isWalkable)
    : _width    (width)
    , _height   (height)
    , _startTile(startTile)
	, _isWalkable(isWalkable)
    , _dist     (width, std::vector<int>(height, -1))
{
	computeDistanceMap(_startTile, isWalkable);
    _sortedTilePositions.reserve(_width * _height);
}

const int & DistanceMap::getDistance(const int & tileX, const int & tileY) const
{ 
    UAB_ASSERT(tileX < _width && tileY < _height, "Index out of range: X = %d, Y = %d", tileX, tileY);
    return _dist[tileX][tileY]; 
}

const int & DistanceMap::getDistance(const BWAPI::Position & pos) const
{ 
    return getDistance(BWAPI::TilePosition(pos)); 
}

const int & DistanceMap::getDistance(const BWAPI::TilePosition & pos) const
{ 
    return getDistance(pos.x, pos.y); 
}

const std::vector<BWAPI::TilePosition> & DistanceMap::getSortedTiles() const
{
    return _sortedTilePositions;
}

// Computes _dist[x][y] = ground distance from (startX, startY) to (x,y)
// Uses BFS, since the map is quite large and DFS may cause a stack overflow
void DistanceMap::computeDistanceMap(const BWAPI::TilePosition & startTile, TileCheckFunc isWalkable)
{
    // the fringe for the BFS we will perform to calculate distances
    std::vector<BWAPI::TilePosition> fringe;
    fringe.reserve(_width * _height);
    fringe.push_back(startTile);
    _sortedTilePositions.push_back(startTile);

    _dist[startTile.x][startTile.y] = 0;

    for (size_t fringeIndex=0; fringeIndex<fringe.size(); ++fringeIndex)
    {
        const BWAPI::TilePosition & tile = fringe[fringeIndex];

        // check every possible child of this tile
        for (size_t a=0; a<LegalActions; ++a)
        {
            BWAPI::TilePosition nextTile(tile.x + actionX[a], tile.y + actionY[a]);

            // if the new tile is inside the map bounds, is walkable, and has not been visited yet, set the distance of its parent + 1
            if (nextTile.isValid() && isWalkable(nextTile) && getDistance(nextTile) == -1)
            {
                _dist[nextTile.x][nextTile.y] = _dist[tile.x][tile.y] + 1;
                fringe.push_back(nextTile);
                _sortedTilePositions.push_back(nextTile);
            }
        }
    }
}