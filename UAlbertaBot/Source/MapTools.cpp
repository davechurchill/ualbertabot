#include "MapTools.h"
#include "Global.h"
#include "Timer.hpp"
#include <utility>
#include <type_traits>

using namespace UAlbertaBot;

const size_t LegalActions = 4;
const int actionX[LegalActions] = {1, -1, 0, 0};
const int actionY[LegalActions] = {0, 0, 1, -1};

// constructor for MapTools
MapTools::MapTools()
    : _width            (BWAPI::Broodwar->mapWidth())
    , _height           (BWAPI::Broodwar->mapHeight())
    , _walkable         (BWAPI::Broodwar->mapWidth(), std::vector<bool>(BWAPI::Broodwar->mapHeight(), false))
    , _buildable        (BWAPI::Broodwar->mapWidth(), std::vector<bool>(BWAPI::Broodwar->mapHeight(), true))
    , _depotBuildable   (BWAPI::Broodwar->mapWidth(), std::vector<bool>(BWAPI::Broodwar->mapHeight(), true))
    , _lastSeen         (BWAPI::Broodwar->mapWidth(), std::vector<int> (BWAPI::Broodwar->mapHeight(), 0))
    , _sectorNumber     (BWAPI::Broodwar->mapWidth(), std::vector<int> (BWAPI::Broodwar->mapHeight(), 0))
{
    setBWAPIMapData();

    computeConnectivity();
}

void MapTools::onStart()
{
    
}

void MapTools::update()
{
    // update all the tiles that we see this frame
    for (size_t x=0; x<_width; ++x)
    {
        for (size_t y=0; y<_height; ++y)
        {
            if (BWAPI::Broodwar->isVisible(BWAPI::TilePosition(x, y)))
            {
                _lastSeen[x][y] = BWAPI::Broodwar->getFrameCount();
            }
        }
    }

    drawLastSeen();
}

void MapTools::computeConnectivity()
{
    // the fringe data structe we will use to do our BFS searches
    std::vector<BWAPI::TilePosition> fringe;
    fringe.reserve(_width*_height);
    int sectorNumber = 0;

    // for every tile on the map, do a connected flood fill using BFS
    for (size_t x=0; x<_width; ++x)
    {
        for (size_t y=0; y<_height; ++y)
        {
            // if the sector is not currently 0, or the map isn't walkable here, then we can skip this tile
            if (_sectorNumber[x][y] != 0 || !_walkable[x][y])
            {
                continue;
            }

            // increase the sector number, so that walkable tiles have sectors 1-N
            sectorNumber++;

            // reset the fringe for the search and add the start tile to it
            fringe.clear();
            fringe.push_back(BWAPI::TilePosition(x,y));
            _sectorNumber[x][y] = sectorNumber;
            
            // do the BFS, stopping when we reach the last element of the fringe
            for (size_t fringeIndex=0; fringeIndex<fringe.size(); ++fringeIndex)
            {
                const BWAPI::TilePosition & tile = fringe[fringeIndex];

                // check every possible child of this tile
                for (size_t a=0; a<LegalActions; ++a)
                {
                    BWAPI::TilePosition nextTile(tile.x + actionX[a], tile.y + actionY[a]);

                    // if the new tile is inside the map bounds, is walkable, and has not been assigned a sector, add it to the current sector and the fringe
                    if (nextTile.isValid() && _walkable[nextTile.x][nextTile.y] && (_sectorNumber[nextTile.x][nextTile.y] == 0))
                    {
                        _sectorNumber[nextTile.x][nextTile.y] = sectorNumber;
                        fringe.push_back(nextTile);
                    }
                }
            }
        }
    }
}

// reads in the map data from bwapi and stores it in our map format
void MapTools::setBWAPIMapData()
{
    // for each row and column
    for (size_t x(0); x < _width; ++x)
    {
        for (size_t y(0); y < _height; ++y)
        {
            bool clear = true;

            // check each walk tile within this TilePosition
            for (int i=0; i<4; ++i)
            {
                for (int j=0; j<4; ++j)
                {
                    if (!BWAPI::Broodwar->isWalkable(x*4 + i, y*4 + j))
                    {
                        clear = false;
                        break;
                    }

                    if (clear)
                    {
                        break;
                    }
                }
            }

            // set the map as binary clear or not
            _walkable[x][y] = clear;

            // set whether this tile is buildable
            _buildable[x][y] = BWAPI::Broodwar->isBuildable(BWAPI::TilePosition(x,y), false);
            _depotBuildable[x][y] = BWAPI::Broodwar->isBuildable(BWAPI::TilePosition(x,y), false);
        }
    }

    // set tiles that static resources are on as unbuildable
    for (auto & resource : BWAPI::Broodwar->getStaticNeutralUnits())
    {
        if (!resource->getType().isResourceContainer())
        {
            continue;
        }

        int tileX = resource->getTilePosition().x;
        int tileY = resource->getTilePosition().y;

        for (int x=tileX; x<tileX+resource->getType().tileWidth(); ++x)
        {
            for (int y=tileY; y<tileY+resource->getType().tileHeight(); ++y)
            {
                _buildable[x][y] = false;

                // depots can't be built within 3 tiles of any resource
                for (int rx=-3; rx<=3; rx++)
                {
                    for (int ry=-3; ry<=3; ry++)
                    {
                        if (!BWAPI::TilePosition(x+rx, y+ry).isValid())
                        {
                            continue;
                        }

                        _depotBuildable[x+rx][y+ry] = false;
                    }
                }
            }
        }
    }
}

bool MapTools::isBuildable(BWAPI::TilePosition tile, BWAPI::UnitType type) const
{
    int startX = tile.x;
    int endX = tile.x + type.tileWidth();
    int startY = tile.y;
    int endY = tile.y + type.tileHeight();

    for (int x=startX; x<endX; ++x)
    {
        for (int y=startY; y<endY; ++y)
        {
            BWAPI::TilePosition tile(x,y);

            if (!isBuildableTile(tile) || (type.isResourceDepot() && !isDepotBuildableTile(tile)))
            {
                return false;
            }
        }
    }

    return true;
}

bool MapTools::isBuildableTile(BWAPI::TilePosition tile) const
{
    if (!tile.isValid())
    {
        return false;
    }

    return _buildable[tile.x][tile.y];
}

bool MapTools::isDepotBuildableTile(BWAPI::TilePosition tile) const
{
    if (!tile.isValid())
    {
        return false;
    }

    return _depotBuildable[tile.x][tile.y];
}

int MapTools::getGroundDistance(const BWAPI::TilePosition & src, const BWAPI::TilePosition & dest) const
{
    if (_allMaps.size() > 50)
    {
        _allMaps.clear();
    }

    return getDistanceMap(dest).getDistance(src);
}


int MapTools::getGroundDistance(const BWAPI::Position & src, const BWAPI::Position & dest) const
{
    return getGroundDistance(BWAPI::TilePosition(src), BWAPI::TilePosition(dest));
}

const DistanceMap & MapTools::getDistanceMap(const BWAPI::TilePosition & tile) const
{
    if (_allMaps.find(tile) == _allMaps.end())
    {
        _allMaps[tile] = DistanceMap(tile);
    }

    return _allMaps[tile];
}

const DistanceMap & MapTools::getDistanceMap(const BWAPI::Position & pos) const
{
    return getDistanceMap(BWAPI::TilePosition(pos));
}

const std::vector<BWAPI::TilePosition> & MapTools::getClosestTilesTo(const BWAPI::TilePosition & tile) const
{
    return getDistanceMap(tile).getSortedTiles();
}

// returns a list of all tiles on the map, sorted by 4-direcitonal walk distance from the given position
const std::vector<BWAPI::TilePosition> & MapTools::getClosestTilesTo(BWAPI::Position pos) const
{
    return getClosestTilesTo(BWAPI::TilePosition(pos));
}

void MapTools::drawLastSeen() const
{
    if (!Config::Debug::DrawLastSeenTileInfo)
    {
        return;
    }

    bool rMouseState = BWAPI::Broodwar->getMouseState(BWAPI::MouseButton::M_RIGHT);
    if (!rMouseState)
    {
        return;
    }

    // draw the least recently seen position tile as a yellow circle on the map
    BWAPI::Position lrsp = getLeastRecentlySeenPosition();
    BWAPI::Broodwar->drawCircleMap(lrsp, 32, BWAPI::Colors::Yellow, true);


    int size = 4;
    BWAPI::Position homePosition(BWAPI::Broodwar->self()->getStartLocation());
    BWAPI::Position mPos = BWAPI::Broodwar->getMousePosition() + BWAPI::Broodwar->getScreenPosition();
    BWAPI::TilePosition mTile(mPos);

    int xStart  = std::max(mTile.x - size, 0);
    int xEnd    = std::min(mTile.x + size, BWAPI::Broodwar->mapWidth());
    int yStart  = std::max(mTile.y - size, 0);
    int yEnd    = std::min(mTile.y + size, BWAPI::Broodwar->mapHeight());

    for (int x = xStart; x < xEnd; ++x)
    {
        for (int y = yStart; y < yEnd; ++y)
        {
            BWAPI::Position pos(x*32, y*32);
            BWAPI::Position diag(32, 32);
            int homeDist = getGroundDistance(pos, homePosition);

            BWAPI::Color boxColor = BWAPI::Colors::Green;
            if (!BWAPI::Broodwar->isBuildable(BWAPI::TilePosition(x,y), true))
            {
                boxColor = BWAPI::Colors::Red;
            }

            BWAPI::Broodwar->drawBoxMap(pos, pos + diag, boxColor, false);
            BWAPI::Broodwar->drawTextMap(pos + BWAPI::Position(2,2), "%d", BWAPI::Broodwar->getFrameCount() - _lastSeen[x][y]);
            BWAPI::Broodwar->drawTextMap(pos + BWAPI::Position(2,12), "%d", homeDist);
        }
    }

}

void MapTools::parseMap()
{
    BWAPI::Broodwar->printf("Parsing Map Information");
    std::ofstream mapFile;
    std::string file = "c:\\scmaps\\" + BWAPI::Broodwar->mapName() + ".txt";
    mapFile.open(file.c_str());

    mapFile << BWAPI::Broodwar->mapWidth()*4 << "\n";
    mapFile << BWAPI::Broodwar->mapHeight()*4 << "\n";

    for (int j=0; j<BWAPI::Broodwar->mapHeight()*4; j++) 
    {
        for (int i=0; i<BWAPI::Broodwar->mapWidth()*4; i++) 
        {
            if (BWAPI::Broodwar->isWalkable(i,j)) 
            {
                mapFile << "0";
            }
            else 
            {
                mapFile << "1";
            }
        }

        mapFile << "\n";
    }

    BWAPI::Broodwar->printf(file.c_str());

    mapFile.close();
}

// get units within radius of center and add to units
void MapTools::GetUnitsInRadius(std::vector<BWAPI::Unit> & units, BWAPI::Position center, int radius, bool ourUnits, bool oppUnits)
{
    const int radiusSquared = radius * radius;

    if (ourUnits)
    {
        for (const BWAPI::Unit & unit : BWAPI::Broodwar->self()->getUnits())
        {
            if (unit->getPosition().getDistance(center) <= radius)
            {
                units.push_back(unit);
            }
        }
    }

    if (oppUnits)
    {
        for (const BWAPI::Unit & unit : BWAPI::Broodwar->enemy()->getUnits())
        {
            if (unit->getPosition().getDistance(center) <= radius)
            {
                units.push_back(unit);
            }
        }
    }
}

const int & MapTools::getSectorNumber(const BWAPI::TilePosition & tile) const
{
    UAB_ASSERT(tile.isValid(), "Getting sector number of invalid tile");

    return _sectorNumber[tile.x][tile.y];
}

int & MapTools::getSectorNumber(const BWAPI::TilePosition & tile)
{
    UAB_ASSERT(tile.isValid(), "Getting sector number of invalid tile");

    return _sectorNumber[tile.x][tile.y];
}

bool MapTools::isWalkable(const BWAPI::TilePosition & tile) const
{
    UAB_ASSERT(tile.isValid(), "Checking walkable of invalid tile");

    return _walkable[tile.x][tile.y];
}

bool MapTools::isWalkable(const BWAPI::Position & pos) const
{
    return isWalkable(BWAPI::TilePosition(pos));
}

bool MapTools::isConnected(const BWAPI::TilePosition & from, const BWAPI::TilePosition & to) const
{
    int sector1 = getSectorNumber(from);
    int sector2 = getSectorNumber(to);

    return (sector1 != 0) && (sector1 == sector2);
}

bool MapTools::isConnected(const BWAPI::Position & from, const BWAPI::Position & to) const
{
    return isConnected(BWAPI::TilePosition(from), BWAPI::TilePosition(to));
}

BWAPI::Position MapTools::getLeastRecentlySeenPosition() const
{
	int minSeen = std::numeric_limits<int>::max();
	BWAPI::TilePosition leastSeen(0,0);
    const BaseLocation * baseLocation = Global::Bases().getPlayerStartingBaseLocation(BWAPI::Broodwar->self());

    if (Global::Bases().getPlayerStartingBaseLocation(BWAPI::Broodwar->enemy()) != nullptr)
    {
        baseLocation = Global::Bases().getPlayerStartingBaseLocation(BWAPI::Broodwar->enemy());
    }

    BWAPI::Position myBasePosition = baseLocation->getPosition();

    for (auto & tile : baseLocation->getClosestTiles())
    {
        UAB_ASSERT(tile.isValid(), "How is this tile not valid?");

		// don't worry about places that aren't connected to our start locatin
		if (!isConnected(BWAPI::Position(tile), myBasePosition))
		{
			continue;
		}

        int lastSeen = _lastSeen[tile.x][tile.y];
		if (lastSeen < minSeen)
		{
			minSeen = lastSeen;
            leastSeen = tile;
		}	
	}

	return BWAPI::Position(leastSeen);
}
