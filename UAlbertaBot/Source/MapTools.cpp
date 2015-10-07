#include "MapTools.h"

using namespace UAlbertaBot;

MapTools & MapTools::Instance()
{
    static MapTools instance;
    return instance;
}

// constructor for MapTools
MapTools::MapTools()
    : _rows(BWAPI::Broodwar->mapHeight())
    , _cols(BWAPI::Broodwar->mapWidth())
{
    _map    = std::vector<bool>(_rows*_cols,false);
    _units  = std::vector<bool>(_rows*_cols,false);
    _fringe = std::vector<int>(_rows*_cols,0);

    setBWAPIMapData();
}

// return the index of the 1D array from (row,col)
inline int MapTools::getIndex(int row,int col)
{
    return row * _cols + col;
}

bool MapTools::unexplored(DistanceMap & dmap,const int index) const
{
    return (index != -1) && dmap[index] == -1 && _map[index];
}

// resets the distance and fringe vectors, call before each search
void MapTools::reset()
{
    std::fill(_fringe.begin(),_fringe.end(),0);
}

// reads in the map data from bwapi and stores it in our map format
void MapTools::setBWAPIMapData()
{
    // for each row and column
    for (int r(0); r < _rows; ++r)
    {
        for (int c(0); c < _cols; ++c)
        {
            bool clear = true;

            // check each walk tile within this TilePosition
            for (int i=0; i<4; ++i)
            {
                for (int j=0; j<4; ++j)
                {
                    if (!BWAPI::Broodwar->isWalkable(c*4 + i,r*4 + j))
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
            _map[getIndex(r,c)] = clear;
        }
    }
}

void MapTools::resetFringe()
{
    std::fill(_fringe.begin(),_fringe.end(),0);
}

int MapTools::getGroundDistance(BWAPI::Position origin,BWAPI::Position destination)
{
    // if we have too many maps, reset our stored maps in case we run out of memory
    if (_allMaps.size() > 20)
    {
        _allMaps.clear();

        BWAPI::Broodwar->printf("Cleared stored distance map cache");
    }

    // if we haven't yet computed the distance map to the destination
    if (_allMaps.find(destination) == _allMaps.end())
    {
        // if we have computed the opposite direction, we can use that too
        if (_allMaps.find(origin) != _allMaps.end())
        {
            return _allMaps[origin][destination];
        }

        // add the map and compute it
        _allMaps.insert(std::pair<BWAPI::Position,DistanceMap>(destination,DistanceMap()));
        computeDistance(_allMaps[destination],destination);
    }

    // get the distance from the map
    return _allMaps[destination][origin];
}

// computes walk distance from Position P to all other points on the map
void MapTools::computeDistance(DistanceMap & dmap,const BWAPI::Position p)
{
    search(dmap,p.y / 32,p.x / 32);
}

// does the dynamic programming search
void MapTools::search(DistanceMap & dmap,const int sR,const int sC)
{
    // reset the internal variables
    resetFringe();

    // set the starting position for this search
    dmap.setStartPosition(sR,sC);

    // set the distance of the start cell to zero
    dmap[getIndex(sR,sC)] = 0;

    // set the fringe variables accordingly
    int fringeSize(1);
    int fringeIndex(0);
    _fringe[0] = getIndex(sR,sC);
    dmap.addSorted(getTilePosition(_fringe[0]));

    // temporary variables used in search loop
    int currentIndex,nextIndex;
    int newDist;

    // the size of the map
    int size = _rows*_cols;

    // while we still have things left to expand
    while (fringeIndex < fringeSize)
    {
        // grab the current index to expand from the fringe
        currentIndex = _fringe[fringeIndex++];
        newDist = dmap[currentIndex] + 1;

        // search up
        nextIndex = (currentIndex > _cols) ? (currentIndex - _cols) : -1;
        if (unexplored(dmap,nextIndex))
        {
            // set the distance based on distance to current cell
            dmap.setDistance(nextIndex,newDist);
            dmap.setMoveTo(nextIndex,'D');
            dmap.addSorted(getTilePosition(nextIndex));

            // put it in the fringe
            _fringe[fringeSize++] = nextIndex;
        }

        // search down
        nextIndex = (currentIndex + _cols < size) ? (currentIndex + _cols) : -1;
        if (unexplored(dmap,nextIndex))
        {
            // set the distance based on distance to current cell
            dmap.setDistance(nextIndex,newDist);
            dmap.setMoveTo(nextIndex,'U');
            dmap.addSorted(getTilePosition(nextIndex));

            // put it in the fringe
            _fringe[fringeSize++] = nextIndex;
        }

        // search left
        nextIndex = (currentIndex % _cols > 0) ? (currentIndex - 1) : -1;
        if (unexplored(dmap,nextIndex))
        {
            // set the distance based on distance to current cell
            dmap.setDistance(nextIndex,newDist);
            dmap.setMoveTo(nextIndex,'R');
            dmap.addSorted(getTilePosition(nextIndex));

            // put it in the fringe
            _fringe[fringeSize++] = nextIndex;
        }

        // search right
        nextIndex = (currentIndex % _cols < _cols - 1) ? (currentIndex + 1) : -1;
        if (unexplored(dmap,nextIndex))
        {
            // set the distance based on distance to current cell
            dmap.setDistance(nextIndex,newDist);
            dmap.setMoveTo(nextIndex,'L');
            dmap.addSorted(getTilePosition(nextIndex));

            // put it in the fringe
            _fringe[fringeSize++] = nextIndex;
        }
    }
}

const std::vector<BWAPI::TilePosition> & MapTools::getClosestTilesTo(BWAPI::Position pos)
{
    // make sure the distance map is calculated with pos as a destination
    int a = getGroundDistance(BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()),pos);

    return _allMaps[pos].getSortedTiles();
}

BWAPI::TilePosition MapTools::getTilePosition(int index)
{
    return BWAPI::TilePosition(index % _cols,index / _cols);
}

BWAPI::TilePosition MapTools::getNextExpansion()
{
    return getNextExpansion(BWAPI::Broodwar->self());
}

void MapTools::drawHomeDistanceMap()
{
    BWAPI::Position homePosition = BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation());
    for (int x = 0; x < BWAPI::Broodwar->mapWidth(); ++x)
    {
        for (int y = 0; y < BWAPI::Broodwar->mapHeight(); ++y)
        {
            BWAPI::Position pos(x*32, y*32);

            int dist = getGroundDistance(pos, homePosition);

            BWAPI::Broodwar->drawTextMap(pos + BWAPI::Position(16,16), "%d", dist);
        }
    }
}

BWAPI::TilePosition MapTools::getNextExpansion(BWAPI::Player player)
{
    BWTA::BaseLocation * closestBase = nullptr;
    double minDistance = 100000;

    BWAPI::TilePosition homeTile = player->getStartLocation();

    // for each base location
    for (BWTA::BaseLocation * base : BWTA::getBaseLocations())
    {
        // if the base has gas
        if (!base->isMineralOnly() && !(base == BWTA::getStartLocation(player)))
        {
            // get the tile position of the base
            BWAPI::TilePosition tile = base->getTilePosition();
            bool buildingInTheWay = false;

            for (int x = 0; x < BWAPI::Broodwar->self()->getRace().getCenter().tileWidth(); ++x)
            {
                for (int y = 0; y < BWAPI::Broodwar->self()->getRace().getCenter().tileHeight(); ++y)
                {
                    BWAPI::TilePosition tp(tile.x + x, tile.y + y);

                    for (auto & unit : BWAPI::Broodwar->getUnitsOnTile(tp))
                    {
                        if (unit->getType().isBuilding() && !unit->isFlying())
                        {
                            buildingInTheWay = true;
                            break;
                        }
                    }
                }
            }
            
            if (buildingInTheWay)
            {
                continue;
            }

            // the base's distance from our main nexus
            BWAPI::Position myBasePosition(player->getStartLocation());
            BWAPI::Position thisTile = BWAPI::Position(tile);
            double distanceFromHome = MapTools::Instance().getGroundDistance(thisTile,myBasePosition);

            // if it is not connected, continue
            if (!BWTA::isConnected(homeTile,tile) || distanceFromHome < 0)
            {
                continue;
            }

            if (!closestBase || distanceFromHome < minDistance)
            {
                closestBase = base;
                minDistance = distanceFromHome;
            }
        }

    }

    if (closestBase)
    {
        return closestBase->getTilePosition();
    }
    else
    {
        return BWAPI::TilePositions::None;
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
