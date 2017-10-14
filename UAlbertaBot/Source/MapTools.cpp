#include "MapTools.h"
#include "Timer.hpp"
#include "UnitUtil.h"
#include <utility>
#include <type_traits>
#include <BWAPI.h>
#include <fstream>

using namespace UAlbertaBot;

const size_t LegalActions = 4;
const int actionX[LegalActions] = {1, -1, 0, 0};
const int actionY[LegalActions] = {0, 0, 1, -1};

// constructor for MapTools
MapTools::MapTools(shared_ptr<AKBot::MapInformation> mapInformation, std::shared_ptr<AKBot::Logger> logger)
    : _width            (mapInformation->getWidth())
    , _height           (mapInformation->getHeight())
	, _mapInformation(mapInformation)
    , _walkable         (mapInformation->getWidth(), std::vector<bool>(mapInformation->getHeight(), false))
    , _buildable        (mapInformation->getWidth(), std::vector<bool>(mapInformation->getHeight(), true))
    , _depotBuildable   (mapInformation->getWidth(), std::vector<bool>(mapInformation->getHeight(), true))
    , _lastSeen         (mapInformation->getWidth(), std::vector<int> (mapInformation->getHeight(), 0))
    , _sectorNumber     (mapInformation->getWidth(), std::vector<int> (mapInformation->getHeight(), 0))
	, _logger(logger)
{
    setBWAPIMapData();

    computeConnectivity();
}

void MapTools::onStart()
{
    
}

void MapTools::update(int currentFrame)
{
    // update all the tiles that we see this frame
	for (size_t x = 0; x < _width; ++x)
	{
		for (size_t y = 0; y < _height; ++y)
		{
			if (_mapInformation->isVisible(x, y))
			{
				_lastSeen[x][y] = currentFrame;
			}
		}
	}
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
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					if (!_mapInformation->isWalkable(x * 4 + i, y * 4 + j))
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
			_buildable[x][y] = _mapInformation->isBuildable(x, y);
			_depotBuildable[x][y] = _mapInformation->isBuildable(x, y);
        }
    }

    // set tiles that static resources are on as unbuildable
    for (auto & resource : BWAPI::Broodwar->getStaticNeutralUnits())
    {
        if (!resource->getType().isResourceContainer())
        {
            continue;
        }

		auto tilePosition = resource->getTilePosition();
        int tileX = tilePosition.x;
        int tileY = tilePosition.y;

		auto resourceType = resource->getType();
		for (int x = tileX; x < tileX + resourceType.tileWidth(); ++x)
		{
			for (int y = tileY; y < tileY + resourceType.tileHeight(); ++y)
			{
				_buildable[x][y] = false;

				// depots can't be built within 3 tiles of any resource
				for (int rx = -3; rx <= 3; rx++)
				{
					for (int ry = -3; ry <= 3; ry++)
					{
						if (!BWAPI::TilePosition(x + rx, y + ry).isValid())
						{
							continue;
						}

						_depotBuildable[x + rx][y + ry] = false;
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
            BWAPI::TilePosition currentTile(x,y);

            if (!isBuildableTile(currentTile) || (type.isResourceDepot() && !isDepotBuildableTile(currentTile)))
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
	auto isWalkable = [this](const BWAPI::TilePosition& tile)
	{ 
		return this->isWalkable(tile);
	};
	if (_allMaps.find(tile) == _allMaps.end())
    {
        _allMaps[tile] = DistanceMap(tile, _width, _height, isWalkable);
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

bool MapTools::isExplored(const BWAPI::TilePosition & tile) const
{
	UAB_ASSERT(tile.isValid(), "Checking explored of invalid tile");

	return _mapInformation->isExplored(tile.x, tile.y);
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

int MapTools::getLastSeen(int x, int y) const
{
	return _lastSeen[x][y];
}