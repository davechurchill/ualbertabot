#include "DMap.h"
#include "MapTools.h"

using namespace UAlbertaBot;

const size_t LegalActions = 4;
const int actionX[LegalActions] = {1, -1, 0, 0};
const int actionY[LegalActions] = {0, 0, 1, -1};

DMap::DMap()
{
    
}

int DMap::getDistance(int tileX, int tileY) const
{ 
    UAB_ASSERT(tileX < m_width && tileY < m_height, "Index out of range: X = %d, Y = %d", tileX, tileY);
    return m_dist[tileX][tileY]; 
}

int DMap::getDistance(const BWAPI::TilePosition & pos) const
{ 
    return getDistance(pos.x, pos.y); 
}

int DMap::getDistance(const BWAPI::Position & pos) const
{ 
    return getDistance(BWAPI::TilePosition(pos));
}

const std::vector<BWAPI::TilePosition> & DMap::getSortedTiles() const
{
    return m_sortedTiles;
}

// Computes m_dist[x][y] = ground distance from (startX, startY) to (x,y)
// Uses BFS, since the map is quite large and DFS may cause a stack overflow
void DMap::computeDistanceMap(const BWAPI::TilePosition & startTile)
{
    m_startTile = startTile;
    m_width = BWAPI::Broodwar->mapWidth();
	m_height = BWAPI::Broodwar->mapHeight();
    m_dist = std::vector<std::vector<int>>(m_width, std::vector<int>(m_height, -1));
    m_sortedTiles.reserve(m_width * m_height);

    // the fringe for the BFS we will perform to calculate distances
	std::vector<BWAPI::TilePosition> fringe;
    fringe.reserve(m_width * m_height);
    fringe.push_back(startTile);
    m_sortedTiles.push_back(startTile);

    m_dist[startTile.x][startTile.y] = 0;

    for (size_t fringeIndex=0; fringeIndex<fringe.size(); ++fringeIndex)
    {
        auto & tile = fringe[fringeIndex];

        // check every possible child of this tile
        for (size_t a=0; a<LegalActions; ++a)
        {
			BWAPI::TilePosition nextTile(tile.x + actionX[a], tile.y + actionY[a]);

            // if the new tile is inside the map bounds, is walkable, and has not been visited yet, set the distance of its parent + 1
            if (MapTools::Instance().isWalkable(nextTile) && getDistance(nextTile) == -1)
            {
                m_dist[(int)nextTile.x][(int)nextTile.y] = m_dist[(int)tile.x][(int)tile.y] + 1;
                fringe.push_back(nextTile);
                m_sortedTiles.push_back(nextTile);
            }
        }
    }
}

void DMap::draw() const
{
    const int tilesToDraw = 200;
    for (size_t i(0); i < tilesToDraw; ++i)
    {
        auto & tile = m_sortedTiles[i];
        int dist = getDistance(tile);

        ///BWAPI::Position textPos(tile.x + Util::TileToPosition(0.5), tile.y + Util::TileToPosition(0.5));
        std::stringstream ss;
        ss << dist;

        //bot.Map().drawText(textPos, ss.str());
    }
}

const BWAPI::TilePosition & DMap::getStartTile() const
{
    return m_startTile;
}