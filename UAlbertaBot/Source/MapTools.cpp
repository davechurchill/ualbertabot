#include "MapTools.h"
#include "BaseLocationManager.h"
#include "Global.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <array>

using namespace UAlbertaBot;

const size_t LegalActions = 4;
const int actionX[LegalActions] ={1, -1, 0, 0};
const int actionY[LegalActions] ={0, 0, 1, -1};

// constructor for MapTools
MapTools::MapTools()
{
    onStart();
}

void MapTools::onStart()
{
    PROFILE_FUNCTION();

    m_width          = BWAPI::Broodwar->mapWidth();
    m_height         = BWAPI::Broodwar->mapHeight();
    m_walkable       = Grid<int>(m_width, m_height, 1);
    m_buildable      = Grid<int>(m_width, m_height, 0);
    m_depotBuildable = Grid<int>(m_width, m_height, 0);
    m_lastSeen       = Grid<int>(m_width, m_height, 0);
    m_sectorNumber   = Grid<int>(m_width, m_height, 0);

    // Set the boolean grid data from the Map
    for (int x(0); x < m_width; ++x)
    {
        for (int y(0); y < m_height; ++y)
        {
            m_buildable.set(x, y, canBuild(x, y));
            m_depotBuildable.set(x, y, canBuild(x, y));
            m_walkable.set(x, y, m_buildable.get(x,y) || canWalk(x, y));
        }
    }

    // set tiles that static resources are on as unbuildable
    for (auto & resource : BWAPI::Broodwar->getStaticNeutralUnits())
    {
        if (!resource->getType().isResourceContainer())
        {
            continue;
        }

        const int tileX = resource->getTilePosition().x;
        const int tileY = resource->getTilePosition().y;

        for (int x=tileX; x<tileX+resource->getType().tileWidth(); ++x)
        {
            for (int y=tileY; y<tileY+resource->getType().tileHeight(); ++y)
            {
                m_buildable.set(x, y, false);

                // depots can't be built within 3 tiles of any resource
                for (int rx=-3; rx<=3; rx++)
                {
                    for (int ry=-3; ry<=3; ry++)
                    {
                        if (!BWAPI::TilePosition(x+rx, y+ry).isValid())
                        {
                            continue;
                        }

                        m_depotBuildable.set(x+rx, y+ry, 0);
                    }
                }
            }
        }
    }

    // compute the map connectivity
    computeConnectivity();
    computeMap();
}

void MapTools::onFrame()
{
    PROFILE_FUNCTION();

    for (int x=0; x<m_width; ++x)
    {
        for (int y=0; y<m_height; ++y)
        {
            if (isVisible(x, y))
            {
                m_lastSeen.set(x, y, m_frame);
            }
        }
    }
    
    m_frame++;
    draw();
}

void MapTools::computeMap()
{
    if (m_map.width() > 0) { return; }

    m_map = StarDraftMap(BWAPI::Broodwar->mapWidth(), BWAPI::Broodwar->mapHeight());

    for (size_t x = 0; x < m_map.width(); x++)
    {
        for (size_t y = 0; y < m_map.height(); y++)
        {
            if      (isDepotBuildableTile(x, y)) { m_map.set(x, y, TileType::BuildAll);   }
            else if (isBuildable(x, y))          { m_map.set(x, y, TileType::NoDepot);    }
            else if (isWalkable(x, y))           { m_map.set(x, y, TileType::Walk);       }
            else                                 { m_map.set(x, y, TileType::Unwalkable); }
        }
    }

    for (auto & mineral : BWAPI::Broodwar->getStaticMinerals())
    {
        const BWAPI::TilePosition mineralTile(mineral->getPosition());
        m_map.set(mineralTile.x, mineralTile.y, TileType::Mineral);
        m_map.set(mineralTile.x-1, mineralTile.y, TileType::Mineral);
    }

    for (auto & geyser : BWAPI::Broodwar->getStaticGeysers())
    {
        const BWAPI::TilePosition geyserTile(geyser->getPosition());
        m_map.set(geyserTile.x, geyserTile.y, TileType::Gas);
        m_map.set(geyserTile.x+1, geyserTile.y, TileType::Gas);
        m_map.set(geyserTile.x-1, geyserTile.y, TileType::Gas);
        m_map.set(geyserTile.x-2, geyserTile.y, TileType::Gas);
        m_map.set(geyserTile.x, geyserTile.y-1, TileType::Gas);
        m_map.set(geyserTile.x+1, geyserTile.y-1, TileType::Gas);
        m_map.set(geyserTile.x-1, geyserTile.y-1, TileType::Gas);
        m_map.set(geyserTile.x-2, geyserTile.y-1, TileType::Gas);
    }

    for (auto & tile : BWAPI::Broodwar->getStartLocations())
    {
        m_map.addStartTile(tile.x, tile.y);
    }

    for (size_t x=0; x<4*m_map.width(); x++)
    {
        for (size_t y=0; y<4*m_map.height(); y++)
        {
            m_map.setWalk(x, y, BWAPI::Broodwar->isWalkable(x,y));
        }
    }
}

void MapTools::computeConnectivity()
{
    PROFILE_FUNCTION();

    // the fringe data structe we will use to do our BFS searches
    std::vector<std::array<int, 2>> fringe;
    fringe.reserve(m_width*m_height);
    int sectorNumber = 0;

    // for every tile on the map, do a connected flood fill using BFS
    for (int x=0; x<m_width; ++x)
    {
        for (int y=0; y<m_height; ++y)
        {
            // if the sector is not currently 0, or the map isn't walkable here, then we can skip this tile
            if (getSectorNumber(x, y) != 0 || !isWalkable(x, y))
            {
                continue;
            }

            // increase the sector number, so that walkable tiles have sectors 1-N
            sectorNumber++;

            // reset the fringe for the search and add the start tile to it
            fringe.clear();
            fringe.push_back({x,y});
            m_sectorNumber.set(x, y, sectorNumber);

            // do the BFS, stopping when we reach the last element of the fringe
            for (size_t fringeIndex=0; fringeIndex<fringe.size(); ++fringeIndex)
            {
                auto & tile = fringe[fringeIndex];

                // check every possible child of this tile
                for (size_t a=0; a<LegalActions; ++a)
                {
                    const int nextX = tile[0] + actionX[a];
                    const int nextY = tile[1] + actionY[a];

                    // if the new tile is inside the map bounds, is walkable, and has not been assigned a sector, add it to the current sector and the fringe
                    if (isValidTile(nextX, nextY) && isWalkable(nextX, nextY) && (getSectorNumber(nextX, nextY) == 0))
                    {
                        m_sectorNumber.set(nextX, nextY, sectorNumber);
                        fringe.push_back({nextX, nextY});
                    }
                }
            }
        }
    }
}

bool MapTools::isExplored(const BWAPI::TilePosition & pos) const
{
    return isExplored(pos.x, pos.y);
}

bool MapTools::isExplored(const BWAPI::Position & pos) const
{
    return isExplored(BWAPI::TilePosition(pos));
}

bool MapTools::isExplored(int tileX, int tileY) const
{
    if (!isValidTile(tileX, tileY)) { return false; }

    return BWAPI::Broodwar->isExplored(tileX, tileY);
}

bool MapTools::isVisible(int tileX, int tileY) const
{
    if (!isValidTile(tileX, tileY)) { return false; }

    return BWAPI::Broodwar->isVisible(BWAPI::TilePosition(tileX, tileY));
}

bool MapTools::isPowered(int tileX, int tileY) const
{
    return BWAPI::Broodwar->hasPower(BWAPI::TilePosition(tileX, tileY));
}

int MapTools::getGroundDistance(const BWAPI::Position & src, const BWAPI::Position & dest) const
{
    if (m_allMaps.size() > 50)
    {
        m_allMaps.clear();
    }

    return getDistanceMap(dest).getDistance(src);
}

const DistanceMap & MapTools::getDistanceMap(const BWAPI::Position & pos) const
{
    return getDistanceMap(BWAPI::TilePosition(pos));
}

const DistanceMap & MapTools::getDistanceMap(const BWAPI::TilePosition & tile) const
{
    std::pair<int,int> pairTile(tile.x, tile.y);

    if (m_allMaps.find(pairTile) == m_allMaps.end())
    {
        m_allMaps[pairTile] = DistanceMap();
        m_allMaps[pairTile].computeDistanceMap(tile);
    }

    return m_allMaps[pairTile];
}

int MapTools::getSectorNumber(int x, int y) const
{
    if (!isValidTile(x, y))
    {
        return 0;
    }

    return m_sectorNumber.get(x, y);
}

bool MapTools::isValidTile(int tileX, int tileY) const
{
    return tileX >= 0 && tileY >= 0 && tileX < m_width && tileY < m_height;
}

bool MapTools::isValidTile(const BWAPI::TilePosition & tile) const
{
    return isValidTile(tile.x, tile.y);
}

bool MapTools::isValidPosition(const BWAPI::Position & pos) const
{
    return isValidTile(BWAPI::TilePosition(pos));
}

bool MapTools::isConnected(int x1, int y1, int x2, int y2) const
{
    if (!isValidTile(x1, y1) || !isValidTile(x2, y2))
    {
        return false;
    }

    const int s1 = getSectorNumber(x1, y1);
    const int s2 = getSectorNumber(x2, y2);

    return s1 != 0 && (s1 == s2);
}

bool MapTools::isConnected(const BWAPI::TilePosition & p1, const BWAPI::TilePosition & p2) const
{
    return isConnected(p1.x, p1.y, p2.x, p2.y);
}

bool MapTools::isConnected(const BWAPI::Position & p1, const BWAPI::Position & p2) const
{
    return isConnected(BWAPI::TilePosition(p1), BWAPI::TilePosition(p2));
}

bool MapTools::isBuildable(int tileX, int tileY) const
{
    if (!isValidTile(tileX, tileY))
    {
        return false;
    }

    return m_buildable.get(tileX, tileY);
}

bool MapTools::canBuildTypeAtPosition(int tileX, int tileY, const BWAPI::UnitType & type) const
{
    return BWAPI::Broodwar->canBuildHere(BWAPI::TilePosition(tileX, tileY), type);
}

bool MapTools::isBuildable(const BWAPI::TilePosition & tile) const
{
    return isBuildable(tile.x, tile.y);
}

void MapTools::printMap() const
{
    std::stringstream ss;
    for (int y(0); y < m_height; ++y)
    {
        for (int x(0); x < m_width; ++x)
        {
            ss << isWalkable(x, y);
        }

        ss << "\n";
    }

    std::ofstream out("map.txt");
    out << ss.str();
    out.close();
}

bool MapTools::isDepotBuildableTile(int tileX, int tileY) const
{
    if (!isValidTile(tileX, tileY))
    {
        return false;
    }

    return m_depotBuildable.get(tileX, tileY);
}

bool MapTools::isWalkable(int tileX, int tileY) const
{
    if (!isValidTile(tileX, tileY))
    {
        return false;
    }

    return m_walkable.get(tileX, tileY);
}

bool MapTools::isWalkable(const BWAPI::TilePosition & tile) const
{
    return isWalkable(tile.x, tile.y);
}

int MapTools::width() const
{
    return m_width;
}

int MapTools::height() const
{
    return m_height;
}

void MapTools::drawTile(int tileX, int tileY, const BWAPI::Color & color) const
{
    const int padding   = 2;
    const int px        = tileX*32 + padding;
    const int py        = tileY*32 + padding;
    const int d         = 32 - 2*padding;

    BWAPI::Broodwar->drawLineMap(px,     py,     px + d, py,     color);
    BWAPI::Broodwar->drawLineMap(px + d, py,     px + d, py + d, color);
    BWAPI::Broodwar->drawLineMap(px + d, py + d, px,     py + d, color);
    BWAPI::Broodwar->drawLineMap(px,     py + d, px,     py,     color);
}

const std::vector<BWAPI::TilePosition> & MapTools::getClosestTilesTo(const BWAPI::TilePosition & tilePos) const
{
    return getDistanceMap(tilePos).getSortedTiles();
}

const std::vector<BWAPI::TilePosition> & MapTools::getClosestTilesTo(const BWAPI::Position & pos) const
{
    return getClosestTilesTo(BWAPI::TilePosition(pos));
}

BWAPI::TilePosition MapTools::getLeastRecentlySeenTile() const
{
    int minSeen = std::numeric_limits<int>::max();
    BWAPI::TilePosition leastSeen;
    const BaseLocation * baseLocation = Global::Bases().getPlayerStartingBaseLocation(BWAPI::Broodwar->self());
    UAB_ASSERT(baseLocation, "Null self baselocation is insanely bad");

    for (auto & tile : baseLocation->getClosestTiles())
    {
        UAB_ASSERT(isValidTile(tile), "How is this tile not valid?");

        const int lastSeen = m_lastSeen.get(tile.x, tile.y);
        if (lastSeen < minSeen)
        {
            minSeen = lastSeen;
            leastSeen = tile;
        }
    }

    return leastSeen;
}

bool MapTools::canWalk(int tileX, int tileY) const
{
    for (int i=0; i<4; ++i)
    {
        for (int j=0; j<4; ++j)
        {
            if (!BWAPI::Broodwar->isWalkable(tileX*4 + i, tileY*4 + j))
            {
                return false;
            }
        }
    }

    return true;
}

bool MapTools::canBuild(int tileX, int tileY) const
{
    return BWAPI::Broodwar->isBuildable(BWAPI::TilePosition(tileX, tileY));
}

void MapTools::draw() const
{
    const BWAPI::TilePosition screen(BWAPI::Broodwar->getScreenPosition());
    const int sx = screen.x;
    const int sy = screen.y;
    const int ex = sx + 20;
    const int ey = sy + 15;
    
    for (int x = sx; x < ex; ++x)
    {
        for (int y = sy; y < ey; y++)
        {
            const BWAPI::TilePosition tilePos(x,y);
            if (!tilePos.isValid()) { continue; }

            if (Config::Debug::DrawWalkableSectors)
            {
                std::stringstream ss;
                ss << getSectorNumber(x, y);
                const BWAPI::Position pos = BWAPI::Position(tilePos) + BWAPI::Position(14,9);
                BWAPI::Broodwar->drawTextMap(pos, ss.str().c_str());
            }

            if (Config::Debug::DrawTileInfo)
            {
                BWAPI::Color color = isWalkable(x, y) ? BWAPI::Color(0, 255, 0) : BWAPI::Color(255, 0, 0);
                if (isWalkable(x, y) && !isBuildable(x, y)) { color = BWAPI::Color(255, 255, 0); }
                if (isBuildable(x, y) && !isDepotBuildableTile(x, y)) { color = BWAPI::Color(127, 255, 255); }
                drawTile(x, y, color);
            }
        }
    }
}

void MapTools::getUnits(BWAPI::Unitset & units, BWAPI::Position center, int radius, bool ourUnits, bool oppUnits)
{
	const int radiusSq(radius * radius);

	if (ourUnits)
	{
		for (auto & unit : BWAPI::Broodwar->self()->getUnits())
		{
			BWAPI::Position d(unit->getPosition() - center);
			if(d.x * d.x + d.y * d.y <= radiusSq)
			{
				if (!units.contains(unit)) 
				{
					units.insert(unit);
				}
			}
		}
	}

	if (oppUnits)
	{
		for (auto & unit : BWAPI::Broodwar->enemy()->getUnits()) 
		{
            if (unit->getType() == BWAPI::UnitTypes::Unknown || !unit->isVisible())
            {
                continue;
            }

			BWAPI::Position d(unit->getPosition() - center);
			if(d.x * d.x + d.y * d.y <= radiusSq)
			{
				if (!units.contains(unit))
				{ 
					units.insert(unit); 
				}
			}
		}
	}
}

const StarDraftMap & MapTools::getStarDraftMap() const
{
    return m_map;
}
    
void MapTools::saveMapToFile(const std::string & path) const
{
    // replace spaces with underscores
    std::string mapFile = BWAPI::Broodwar->mapFileName();
    std::replace( mapFile.begin(), mapFile.end(), ' ', '_'); 
    getStarDraftMap().save(mapFile + ".txt");
}