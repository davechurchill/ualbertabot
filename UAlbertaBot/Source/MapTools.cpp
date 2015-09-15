#include "MapTools.h"

using namespace UAlbertaBot;

MapTools & MapTools::Instance() 
{
	static MapTools instance;
	return instance;
}

// constructor for MapTools
MapTools::MapTools() 
	: rows(BWAPI::Broodwar->mapHeight())
	, cols(BWAPI::Broodwar->mapWidth())		
{
	map			= std::vector<bool>(rows*cols, false);
	units		= std::vector<bool>(rows*cols, false);
	fringe		= std::vector<int> (rows*cols, 0);

	setBWAPIMapData();
}

// return the index of the 1D array from (row,col)
inline int MapTools::getIndex(int row, int col)
{
	return row * cols + col;
}

bool MapTools::unexplored(DistanceMap & dmap, const int index) const
{
	return (index != -1) && dmap[index] == -1 && map[index];
}

// resets the distance and fringe vectors, call before each search
void MapTools::reset()
{
	std::fill(fringe.begin(), fringe.end(), 0);
}

// reads in the map data from bwapi and stores it in our map format
void MapTools::setBWAPIMapData()
{
	// for each row and column
	for (int r(0); r < rows; ++r)
	{
		for (int c(0); c < cols; ++c)
		{
			bool clear = false;

			// check each walk tile within this TilePosition
			for (int i=0; i<4; ++i)
			{
				for (int j=0; j<4; ++j)
				{
					if (BWAPI::Broodwar->isWalkable(c*4 + i, r*4 + j))
					{
						clear = true;
						break;
					}

					if (clear)
					{
						break;
					}
				}
			}
			
			// set the map as binary clear or not
			map[getIndex(r,c)] = clear;
		}
	}
}

void MapTools::resetFringe()
{
	std::fill(fringe.begin(), fringe.end(), 0);
}

int MapTools::getGroundDistance(BWAPI::Position origin, BWAPI::Position destination)
{
	// if we haven't yet computed the distance map to the destination
	if (allMaps.find(destination) == allMaps.end())
	{
        // if we have computed the opposite direction, we can use that too
		if (allMaps.find(origin) != allMaps.end())
        {
            return allMaps[origin][destination];
        }
        
        // add the map and compute it
		allMaps[destination] = DistanceMap();
		computeDistance(allMaps[destination], destination);
	}

	// get the distance from the map
	return allMaps[destination][origin];
}

// computes walk distance from Position P to all other points on the map
void MapTools::computeDistance(DistanceMap & dmap, const BWAPI::Position p)
{
	search(dmap, p.y / 32, p.x / 32);
}

// does the dynamic programming search
void MapTools::search(DistanceMap & dmap, const int sR, const int sC)
{
	// reset the internal variables
	resetFringe();

	// set the starting position for this search
	dmap.setStartPosition(sR,sC);
	
	// set the distance of the start cell to zero
	dmap[getIndex(sR, sC)] = 0;
	
	// set the fringe variables accordingly
	int fringeSize(1);
	int fringeIndex(0);
	fringe[0] = getIndex(sR,sC);
    dmap.addSorted(getTilePosition(fringe[0]));
	
	// temporary variables used in search loop
	int currentIndex, nextIndex;
	int newDist;
	
	// the size of the map
	int size = rows*cols;
	
	// while we still have things left to expand
	while (fringeIndex < fringeSize)
	{
		// grab the current index to expand from the fringe
		currentIndex = fringe[fringeIndex++];
		newDist = dmap[currentIndex] + 1;
		
		// search up
		nextIndex = (currentIndex > cols) ? (currentIndex - cols) : -1;
		if (unexplored(dmap, nextIndex))
		{
			// set the distance based on distance to current cell
			dmap.setDistance(nextIndex, newDist);
			dmap.setMoveTo(nextIndex, 'D');
            dmap.addSorted(getTilePosition(nextIndex));
				
			// put it in the fringe
			fringe[fringeSize++] = nextIndex;				
		}
		
		// search down
		nextIndex = (currentIndex + cols < size) ? (currentIndex + cols) : -1;
		if (unexplored(dmap, nextIndex))
		{
			// set the distance based on distance to current cell
			dmap.setDistance(nextIndex, newDist);
			dmap.setMoveTo(nextIndex, 'U');
            dmap.addSorted(getTilePosition(nextIndex));

			// put it in the fringe
			fringe[fringeSize++] = nextIndex;
		}
		
		// search left
		nextIndex = (currentIndex % cols > 0) ? (currentIndex - 1) : -1;
		if (unexplored(dmap, nextIndex))
		{
			// set the distance based on distance to current cell
			dmap.setDistance(nextIndex, newDist);
			dmap.setMoveTo(nextIndex, 'R');
            dmap.addSorted(getTilePosition(nextIndex));

			// put it in the fringe
			fringe[fringeSize++] = nextIndex;
		}
		
		// search right
		nextIndex = (currentIndex % cols < cols - 1) ? (currentIndex + 1) : -1;
		if (unexplored(dmap, nextIndex))
		{
			// set the distance based on distance to current cell
			dmap.setDistance(nextIndex, newDist);
			dmap.setMoveTo(nextIndex, 'L');
            dmap.addSorted(getTilePosition(nextIndex));

			// put it in the fringe
			fringe[fringeSize++] = nextIndex;
		}
	}
}

const std::vector<BWAPI::TilePosition> & MapTools::getClosestTilesTo(BWAPI::Position pos)
{
    // make sure the distance map is calculated with pos as a destination
    int a = getGroundDistance(BWAPI::Position(0,0), pos);

    return allMaps[pos].getSortedTiles();
}

BWAPI::TilePosition MapTools::getTilePosition(int index)
{
    return BWAPI::TilePosition(index % cols, index / cols);
}

std::vector<BWAPI::UnitInterface *> MapTools::getUnitsOfTypeNear(BWAPI::Position & nearTo, int groundDistance, BWAPI::UnitType type)
{
    std::vector<BWAPI::UnitInterface *> unitsNear;

    auto it =   std::copy_if
                ( 
                    BWAPI::Broodwar->getAllUnits().begin(), 
                    BWAPI::Broodwar->getAllUnits().end(), 
                    unitsNear.begin(), 
                    [this, &type, &nearTo, &groundDistance](BWAPI::Unit unit)
                    { 
                        return (unit->getType() == type) && (getGroundDistance(unit->getPosition(), nearTo) < groundDistance ); 
                    } 
                );

    return unitsNear;
}

BWAPI::TilePosition MapTools::getNextExpansion()
{
	return getNextExpansion(BWAPI::Broodwar->self());
}

BWAPI::TilePosition MapTools::getNextExpansion(BWAPI::Player player)
{
	BWTA::BaseLocation * closestBase = NULL;
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

			// the rectangle for this base location
			int x1 = tile.x * 32;
			int y1 = tile.y * 32;
			int x2, y2;
			switch (player->getRace().getID()){
			case BWAPI::Races::Enum::Zerg:
				x2 = x1 + BWAPI::UnitTypes::Zerg_Hatchery.tileWidth() * 32;
				y2 = y1 + BWAPI::UnitTypes::Zerg_Hatchery.tileHeight() * 32;
				break;
			case BWAPI::Races::Enum::Terran:
				x2 = x1 + BWAPI::UnitTypes::Terran_Command_Center.tileWidth() * 32;
				y2 = y1 + BWAPI::UnitTypes::Terran_Command_Center.tileHeight() * 32;
				break;
			case BWAPI::Races::Enum::Protoss:
				x2 = x1 + BWAPI::UnitTypes::Protoss_Nexus.tileWidth() * 32;
				y2 = y1 + BWAPI::UnitTypes::Protoss_Nexus.tileHeight() * 32;
				break;
			default:
				UAB_ASSERT(false, "Unknown race when trying to find next base location");
			}

			bool buildingInTheWay = false;

			// for each unit in the rectangle where we want to build it
			for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->getUnitsInRectangle(x1, y1, x2, y2))
			{
				// if the unit is a building, we can't build here
				if (unit->getType().isBuilding())
				{
					buildingInTheWay = true;
					break;
				}
			}

			if (buildingInTheWay)
			{
				continue;
			}

			// the base's distance from our main nexus
			BWAPI::Position myBasePosition(player->getStartLocation());
            BWAPI::Position thisTile = BWAPI::Position(tile);
		    double distanceFromHome = MapTools::Instance().getGroundDistance(thisTile, myBasePosition);

			// if it is not connected, continue
			if (!BWTA::isConnected(homeTile, tile) || distanceFromHome < 0)
			{
				continue;
			}

			if(!closestBase || distanceFromHome < minDistance)
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

	for (int j=0; j<BWAPI::Broodwar->mapHeight()*4; j++) {
		for (int i=0; i<BWAPI::Broodwar->mapWidth()*4; i++) {

			if (BWAPI::Broodwar->isWalkable(i,j)) {
				mapFile << "0";
			} else {
				mapFile << "1";
			}
		}

		mapFile << "\n";
	}
	
	BWAPI::Broodwar->printf(file.c_str());

	mapFile.close();
}
