#include "Common.h"
#include "BuildingPlacer.h"
#include "MapGrid.h"

using namespace UAlbertaBot;

BuildingPlacer::BuildingPlacer() 
    : boxTop(100000)
    , boxBottom(-1)
    , boxLeft(100000)
    , boxRight(-1)
{
	reserveMap = std::vector< std::vector<bool> >(BWAPI::Broodwar->mapWidth(), std::vector<bool>(BWAPI::Broodwar->mapHeight(), false));
	buildDistance = 0;

	computeResourceBox();
}

bool BuildingPlacer::isInResourceBox(int x, int y) const
{
	int posX(x*32);
	int posY(y*32);

	if (posX >= boxLeft && posX < boxRight && posY >= boxTop && posY < boxBottom)
	{
		return true;
	}

	return false;
}

void BuildingPlacer::computeResourceBox()
{
	BWAPI::Position start(BWAPI::Broodwar->self()->getStartLocation());
	std::vector<BWAPI::UnitInterface*> unitsAroundNexus;

	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->getAllUnits())
	{
		// if the units are less than 400 away add them if they are resources
		if (unit->getDistance(start) < 400 && unit->getType().isResourceContainer())
		{
			unitsAroundNexus.push_back(unit);
		}
	}

	for (BWAPI::UnitInterface* unit : unitsAroundNexus)
	{
		int x = unit->getPosition().x;
		int y = unit->getPosition().y;

		int left = x - unit->getType().dimensionLeft();
		int right = x + unit->getType().dimensionRight() + 1;
		int top = y - unit->getType().dimensionUp();
		int bottom = y + unit->getType().dimensionDown() + 1;

		boxTop = top < boxTop ? top : boxTop;
		boxBottom = bottom > boxBottom ? bottom : boxBottom;
		boxLeft = left < boxLeft ? left : boxLeft;
		boxRight = right > boxRight ? right : boxRight;
	}

	//BWAPI::Broodwar->printf("%d %d %d %d", boxTop, boxBottom, boxLeft, boxRight);
}

// makes final checks to see if a building can be built at a certain location
bool BuildingPlacer::canBuildHere(BWAPI::TilePosition position, const Building & b) const
{
	/*if (!b.type.isRefinery() && !InformationManager::Instance().tileContainsUnit(position))
	{
		return false;
	}*/

	//returns true if we can build this type of unit here. Takes into account reserved tiles.
	if (!BWAPI::Broodwar->canBuildHere(position, b.type, b.builderUnit))
	{
		return false;
	}

	// check the reserve map
	for(int x = position.x; x < position.x + b.type.tileWidth(); x++)
	{
		for(int y = position.y; y < position.y + b.type.tileHeight(); y++)
		{
			if (reserveMap[x][y])
			{
				return false;
			}
		}
	}

	// if it overlaps a base location return false
	if (tileOverlapsBaseLocation(position, b.type))
	{
		return false;
	}

	return true;
}

bool BuildingPlacer::tileBlocksAddon(BWAPI::TilePosition position) const
{
    
    for (int i=0; i<=2; ++i)
    {
        for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->getUnitsOnTile(position.x - i, position.y))
        {
            if ( unit->getType() == BWAPI::UnitTypes::Terran_Command_Center ||
		        unit->getType() == BWAPI::UnitTypes::Terran_Factory || 
		        unit->getType() == BWAPI::UnitTypes::Terran_Starport ||
		        unit->getType() == BWAPI::UnitTypes::Terran_Science_Facility)
	        {
		        return true;
	        }
        }
    }

    return false;
}

//returns true if we can build this type of unit here with the specified amount of space.
//space value is stored in this->buildDistance.
bool BuildingPlacer::canBuildHereWithSpace(BWAPI::TilePosition position, const Building & b, int buildDist, bool horizontalOnly) const
{
    BWAPI::UnitType type = b.type;

	//if we can't build here, we of course can't build here with space
	if (!canBuildHere(position, b))
	{
		return false;
	}

	// height and width of the building
	int width(b.type.tileWidth());
	int height(b.type.tileHeight());

	//make sure we leave space for add-ons. These types of units can have addons:
	if (b.type==BWAPI::UnitTypes::Terran_Command_Center ||
		b.type==BWAPI::UnitTypes::Terran_Factory || 
		b.type==BWAPI::UnitTypes::Terran_Starport ||
		b.type==BWAPI::UnitTypes::Terran_Science_Facility)
	{
		width += 2;
	}

	// define the rectangle of the building spot
	int startx = position.x - buildDist;
	int starty = position.y - buildDist;
	int endx   = position.x + width + buildDist;
	int endy   = position.y + height + buildDist;

    if (b.type.isAddon())
    {
        const BWAPI::UnitType builderType = type.whatBuilds().first;

        BWAPI::TilePosition builderTile(position.x - builderType.tileWidth(), position.y + 2 - builderType.tileHeight());

        startx = builderTile.x - buildDist;
        starty = builderTile.y - buildDist;
        endx = position.x + width + buildDist;
        endy = position.y + height + buildDist;
    }

	if (horizontalOnly)
	{
		starty += buildDist;
		endy -= buildDist;
	}

	// if this rectangle doesn't fit on the map we can't build here
	if (startx < 0 || starty < 0 || endx > BWAPI::Broodwar->mapWidth() || endx < position.x + width || endy > BWAPI::Broodwar->mapHeight()) 
	{
		return false;
	}

	// if we can't build here, or space is reserved, or it's in the resource box, we can't build here
	for(int x = startx; x < endx; x++)
	{
		for(int y = starty; y < endy; y++)
		{
			if (!b.type.isRefinery())
			{
				if (!buildable(b, x, y) || reserveMap[x][y] || ((b.type != BWAPI::UnitTypes::Protoss_Photon_Cannon) && isInResourceBox(x,y)))
				{
					return false;
				}
			}
		}
	}

	return true;
}

BWAPI::TilePosition BuildingPlacer::GetBuildLocation(const Building & b, int padding) const
{
    return BWAPI::TilePosition(0,0);
}

BWAPI::TilePosition BuildingPlacer::getBuildLocationNear(const Building & b, int buildDist, bool horizontalOnly) const
{
    SparCraft::Timer t;
    t.start();

    // get the precomputed vector of tile positions which are sorted closes to this location
    const std::vector<BWAPI::TilePosition> & closestToBuilding = MapTools::Instance().getClosestTilesTo(BWAPI::Position(b.desiredPosition));

    double ms1 = t.getElapsedTimeInMilliSec();

    // special easy case of having no pylons
    int numPylons = BWAPI::Broodwar->self()->completedUnitCount(BWAPI::UnitTypes::Protoss_Pylon);
    if (b.type.requiresPsi() && numPylons == 0)
    {
        return BWAPI::TilePositions::None;
    }

    // iterate through the list until we've found a suitable location
    for (size_t i(0); i < closestToBuilding.size(); ++i)
    {
        if (canBuildHereWithSpace(closestToBuilding[i], b, buildDist, horizontalOnly))
        {
            double ms = t.getElapsedTimeInMilliSec();
            //BWAPI::Broodwar->printf("Building Placer Took %d iterations, lasting %lf ms @ %lf iterations/ms, %lf setup ms", i, ms, (i / ms), ms1);

            return closestToBuilding[i];
        }
    }

    double ms = t.getElapsedTimeInMilliSec();
    //BWAPI::Broodwar->printf("Building Placer Took %lf ms", ms);

	return  BWAPI::TilePositions::None;
}

bool BuildingPlacer::tileOverlapsBaseLocation(BWAPI::TilePosition tile, BWAPI::UnitType type) const
{
	// if it's a resource depot we don't care if it overlaps
	if (type.isResourceDepot())
	{
		return false;
	}

	// dimensions of the proposed location
	int tx1 = tile.x;
	int ty1 = tile.y;
	int tx2 = tx1 + type.tileWidth();
	int ty2 = ty1 + type.tileHeight();

	// for each base location
	for (BWTA::BaseLocation * base : BWTA::getBaseLocations())
	{
		// dimensions of the base location
		int bx1 = base->getTilePosition().x;
		int by1 = base->getTilePosition().y;
		int bx2 = bx1 + BWAPI::Broodwar->self()->getRace().getCenter().tileWidth();
		int by2 = by1 + BWAPI::Broodwar->self()->getRace().getCenter().tileHeight();

		// conditions for non-overlap are easy
		bool noOverlap = (tx2 < bx1) || (tx1 > bx2) || (ty2 < by1) || (ty1 > by2);

		// if the reverse is true, return true
		if (!noOverlap)
		{
			return true;
		}
	}

	// otherwise there is no overlap
	return false;
}

bool BuildingPlacer::buildable(const Building & b, int x, int y) const
{
    BWAPI::TilePosition tp(x,y);

	//returns true if this tile is currently buildable, takes into account units on tile
	if (!BWAPI::Broodwar->isBuildable(x,y)) 
	{
		return false;
	}
    
    if ((BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran) && tileBlocksAddon(BWAPI::TilePosition(x, y)))
    {
        return false;
    }

    for (BWAPI::UnitInterface * unit : BWAPI::Broodwar->getUnitsOnTile(x, y))
    {
        if ((b.builderUnit != NULL) && (unit != b.builderUnit))
        {
            return false;
        }
    }

    if (!tp.isValid())
    {
        return false;
    }

	return true;
}

void BuildingPlacer::reserveTiles(BWAPI::TilePosition position, int width, int height)
{
	int rwidth = reserveMap.size();
	int rheight = reserveMap[0].size();
	for(int x = position.x; x < position.x + width && x < rwidth; x++) 
	{
		for(int y = position.y; y < position.y + height && y < rheight; y++) 
		{
			reserveMap[x][y] = true;
		}
	}
}

void BuildingPlacer::drawReservedTiles()
{
    if (!Config::Debug::DrawReservedBuildingTiles)
    {
        return;
    }

	int rwidth = reserveMap.size();
	int rheight = reserveMap[0].size();

	for(int x = 0; x < rwidth; ++x) 
	{
		for(int y = 0; y < rheight; ++y) 
		{
			if (reserveMap[x][y] || isInResourceBox(x,y))
			{
				int x1 = x*32 + 8;
				int y1 = y*32 + 8;
				int x2 = (x+1)*32 - 8;
				int y2 = (y+1)*32 - 8;

				 BWAPI::Broodwar->drawBoxMap(x1, y1, x2, y2, BWAPI::Colors::Yellow, false);
			}
		}
	}

    const std::vector<BWAPI::TilePosition> & closestToBuilding = MapTools::Instance().getClosestTilesTo(BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()));
    Building b(BWAPI::UnitTypes::Protoss_Pylon, BWAPI::Broodwar->self()->getStartLocation());

    for (size_t i(0); i < 400; ++i)
    {
        int x = closestToBuilding[i].x;
        int y = closestToBuilding[i].y;

        if (!(reserveMap[x][y] || isInResourceBox(x,y)) && canBuildHere(closestToBuilding[i], b))
        {
            int x1 = x*32 + 1;
			int y1 = y*32 + 1;
			int x2 = (x+1)*32 - 1;
			int y2 = (y+1)*32 - 1;

            BWAPI::Broodwar->drawTextMap(x1+3, y1+2, "%d", i);
            BWAPI::Broodwar->drawBoxMap(x1, y1, x2, y2, BWAPI::Colors::Green, false);
        }
    }
}

void BuildingPlacer::freeTiles(BWAPI::TilePosition position, int width, int height)
{
	int rwidth = reserveMap.size();
	int rheight = reserveMap[0].size();

	for(int x = position.x; x < position.x + width && x < rwidth; x++) 
	{
		for(int y = position.y; y < position.y + height && y < rheight; y++) 
		{
			reserveMap[x][y] = false;
		}
	}
}

void BuildingPlacer::setBuildDistance(int distance)
{
	this->buildDistance=distance;
}

int BuildingPlacer::getBuildDistance() const
{
	return this->buildDistance;
}

BWAPI::TilePosition BuildingPlacer::getRefineryPosition()
{
	// for each of our units
	for (BWAPI::UnitInterface* depot : BWAPI::Broodwar->self()->getUnits())
	{
		// if it's a resource depot
		if (depot->getType().isResourceDepot())
		{
			// for all units around it
			for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->getAllUnits())
			{
				// if it's a geyser around it
				if (unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser && unit->getDistance(depot) < 300)
				{
					return unit->getTilePosition();
				}
			}
		}
	}

	return BWAPI::TilePositions::None;
}

bool BuildingPlacer::isReserved(int x, int y) const
{
	int rwidth = reserveMap.size();
	int rheight = reserveMap[0].size();
	if (x < 0 || y < 0 || x >= rwidth || y >= rheight) 
	{
		return false;
	}

	return reserveMap[x][y];
}

BuildingPlacer & BuildingPlacer::Instance() {

	static BuildingPlacer instance;
	return instance;
}

