#include "DistanceMap.h"
#include "MapTools.h"
#include "Global.h"

using namespace UAlbertaBot;

const size_t LegalActions = 4;
const int actionX[LegalActions] ={1, -1, 0, 0};
const int actionY[LegalActions] ={0, 0, 1, -1};

DistanceMap::DistanceMap()
{

}

int DistanceMap::getDistance(int tileX, int tileY) const
{
    UAB_ASSERT(tileX < m_width && tileY < m_height, "Index out of range: X = %d, Y = %d", tileX, tileY);

    return m_dist.get(tileX, tileY);
}

int DistanceMap::getDistance(const BWAPI::TilePosition & pos) const
{
    return getDistance(pos.x, pos.y);
}

int DistanceMap::getDistance(const BWAPI::Position & pos) const
{
    return getDistance(BWAPI::TilePosition(pos));
}

const std::vector<BWAPI::TilePosition> & DistanceMap::getSortedTiles() const
{
    return m_sortedTiles;
}

// Computes m_dist[x][y] = ground distance from (startX, startY) to (x,y)
// Uses BFS, since the map is quite large and DFS may cause a stack overflow
void DistanceMap::computeDistanceMap(const BWAPI::TilePosition & startTile)
{
    PROFILE_FUNCTION();

    m_startTile = startTile;
    m_width     = BWAPI::Broodwar->mapWidth();
    m_height    = BWAPI::Broodwar->mapHeight();
    m_dist      = Grid<int>(m_width, m_height, -1);
    m_sortedTiles.reserve(m_width * m_height);

    // the fringe for the BFS we will perform to calculate distances
    std::vector<BWAPI::TilePosition> fringe;
    fringe.reserve(m_width * m_height);
    fringe.push_back(startTile);
    m_sortedTiles.push_back(startTile);

    m_dist.set(startTile.x, startTile.y, 0);

    for (size_t fringeIndex=0; fringeIndex<fringe.size(); ++fringeIndex)
    {
        const auto & tile = fringe[fringeIndex];

        // check every possible child of this tile
        for (size_t a=0; a<LegalActions; ++a)
        {
            const BWAPI::TilePosition nextTile(tile.x + actionX[a], tile.y + actionY[a]);

            // if the new tile is inside the map bounds, is walkable, and has not been visited yet, set the distance of its parent + 1
            if (Global::Map().isWalkable(nextTile) && getDistance(nextTile) == -1)
            {
                m_dist.set(nextTile.x, nextTile.y, m_dist.get(tile.x, tile.y) + 1);
                fringe.push_back(nextTile);
                m_sortedTiles.push_back(nextTile);
            }
        }
    }
}

void DistanceMap::draw() const
{
    const int tilesToDraw = 200;
    for (size_t i(0); i < tilesToDraw && i < m_sortedTiles.size(); ++i)
    {
        const auto & tile = m_sortedTiles[i];
        const int dist = getDistance(tile);

        const BWAPI::Position textPos(tile);
        std::stringstream ss;
        ss << dist;

        BWAPI::Broodwar->drawTextMap(textPos, ss.str().c_str(), BWAPI::Colors::White);
    }
}

const BWAPI::TilePosition & DistanceMap::getStartTile() const
{
    return m_startTile;
}