#include "MapTools.h"
#include "Global.h"

using namespace UAlbertaBot;

// constructor for MapTools
MapTools::MapTools()
    : _rows(BWAPI::Broodwar->mapHeight())
    , _cols(BWAPI::Broodwar->mapWidth())
{
    _map        = std::vector<bool>(_rows*_cols, false);
    _units      = std::vector<bool>(_rows*_cols, false);
    _lastSeen   = std::vector<int>(_rows*_cols, 0);
    _buildable  = std::vector<bool>(_rows*_cols, true);
    _depotBuildable = std::vector<bool>(_rows*_cols, true);

    setBWAPIMapData();
}

void MapTools::onStart()
{
    
}

void MapTools::update()
{
    // update all the tiles that we see this frame
    for (int r=0; r<_rows; ++r)
    {
        for (int c=0; c<_cols; ++c)
        {
            if (BWAPI::Broodwar->isVisible(BWAPI::TilePosition(c, r)))
            {
                _lastSeen[getIndex(r,c)] = BWAPI::Broodwar->getFrameCount();
            }
        }
    }

    drawLastSeen();
}

// return the index of the 1D array from (row,col)
int MapTools::getIndex(int row,int col) const
{
    int index = row * _cols + col;

    UAB_ASSERT(index < _map.size(), "getIndex returning larger index than possible: %d", index);

    return index;
}

bool MapTools::unexplored(DistanceMap & dmap,const int index) const
{
    return (index != -1) && dmap[index] == -1 && _map[index];
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

            // set whether this tile is buildable
            _buildable[getIndex(r,c)] = BWAPI::Broodwar->isBuildable(BWAPI::TilePosition(c,r), false);
            _depotBuildable[getIndex(r,c)] = BWAPI::Broodwar->isBuildable(BWAPI::TilePosition(c,r), false);
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
                _buildable[getIndex(y,x)] = false;

                // depots can't be built within 3 tiles of any resource
                for (int rx=-3; rx<=3; rx++)
                {
                    for (int ry=-3; ry<=3; ry++)
                    {
                        if (!BWAPI::TilePosition(x+rx, y+ry).isValid())
                        {
                            continue;
                        }

                        _depotBuildable[getIndex(y+ry,x+rx)] = false;
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

    return _buildable[getIndex(tile.y, tile.x)];
}

bool MapTools::isDepotBuildableTile(BWAPI::TilePosition tile) const
{
    if (!tile.isValid())
    {
        return false;
    }

    return _depotBuildable[getIndex(tile.y, tile.x)];
}

int MapTools::getGroundDistance(BWAPI::Position origin,BWAPI::Position destination) const
{
    // if we have too many maps, reset our stored maps in case we run out of memory
    if (_allMaps.size() > 50)
    {
        _allMaps.clear();

        //BWAPI::Broodwar->printf("Cleared stored distance map cache");
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
void MapTools::computeDistance(DistanceMap & dmap, const BWAPI::Position p) const
{
    search(dmap,p.y / 32,p.x / 32);
}

// does the dynamic programming search
void MapTools::search(DistanceMap & dmap,const int sR,const int sC) const
{
    // set the starting position for this search
    dmap.setStartPosition(sR,sC);

    // set the distance of the start cell to zero
    dmap[getIndex(sR,sC)] = 0;

    // set the fringe variables accordingly
    int fringeSize(1);
    int fringeIndex(0);
    std::vector<int> fringe(_rows*_cols, 0);
    fringe[0] = getIndex(sR,sC);
    dmap.addSorted(getTilePosition(fringe[0]));

    // temporary variables used in search loop
    int currentIndex,nextIndex;
    int newDist;

    // the size of the map
    int size = _rows*_cols;

    // while we still have things left to expand
    while (fringeIndex < fringeSize)
    {
        // grab the current index to expand from the fringe
        currentIndex = fringe[fringeIndex++];
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
            fringe[fringeSize++] = nextIndex;
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
            fringe[fringeSize++] = nextIndex;
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
            fringe[fringeSize++] = nextIndex;
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
            fringe[fringeSize++] = nextIndex;
        }
    }
}

// returns a list of all tiles on the map, sorted by 4-direcitonal walk distance from the given position
const std::vector<BWAPI::TilePosition> & MapTools::getClosestTilesTo(BWAPI::Position pos) const
{
    // if we haven't yet computed the distance map to the position (as a destination), do it
    if (_allMaps.find(pos) == _allMaps.end())
    {
        // add the map and compute it
        _allMaps.insert(std::pair<BWAPI::Position,DistanceMap>(pos, DistanceMap()));
        computeDistance(_allMaps[pos], pos);
    }

    return _allMaps[pos].getSortedTiles();
}

BWAPI::TilePosition MapTools::getTilePosition(int index) const
{
    return BWAPI::TilePosition(index % _cols,index / _cols);
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
            BWAPI::Broodwar->drawTextMap(pos + BWAPI::Position(2,2), "%d", BWAPI::Broodwar->getFrameCount() - _lastSeen[getIndex(y, x)]);
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

bool MapTools::isConnected(BWAPI::Position from, BWAPI::Position to) const
{
    return getGroundDistance(from, to) != -1;
}

BWAPI::Position MapTools::getLeastRecentlySeenPosition() const
{
	int minSeen = std::numeric_limits<int>::max();
	BWAPI::TilePosition leastSeen(0,0);
    const BaseLocation * myBaseLocation = Global::Bases().getPlayerStartingBaseLocation(BWAPI::Broodwar->self());
    BWAPI::Position myBasePosition = myBaseLocation->getPosition();

    for (auto & tile : myBaseLocation->getClosestTiles())
    {
        UAB_ASSERT(tile.isValid(), "How is this tile not valid?");

		// don't worry about places that aren't connected to our start locatin
		if (!isConnected(BWAPI::Position(tile), myBasePosition))
		{
			continue;
		}

        int lastSeen = _lastSeen[getIndex(tile.y,tile.x)];
		if (lastSeen < minSeen)
		{
			minSeen = lastSeen;
            leastSeen = tile;
		}	
	}

	return BWAPI::Position(leastSeen);
}
