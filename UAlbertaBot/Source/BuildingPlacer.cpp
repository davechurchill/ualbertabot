#include "Common.h"
#include "BuildingPlacer.h"
#include "UnitUtil.h"

using namespace UAlbertaBot;

BuildingPlacer::BuildingPlacer(
	int width,
	int height,
	shared_ptr<AKBot::OpponentView> opponentView,
	shared_ptr<BaseLocationManager> bases,
	shared_ptr<MapTools> mapTools)
    : _boxTop       (std::numeric_limits<int>::max())
    , _boxBottom    (std::numeric_limits<int>::lowest())
    , _boxLeft      (std::numeric_limits<int>::max())
    , _boxRight     (std::numeric_limits<int>::lowest())
	, _width(width)
	, _height(height)
	, _opponentView(opponentView)
	, _bases(bases)
	, _mapTools(mapTools)
{
    _reserveMap = std::vector< std::vector<bool> >(width,std::vector<bool>(height,false));

    computeResourceBox();
}

bool BuildingPlacer::isInResourceBox(int x, int y) const
{
    int posX = x * 32;
    int posY = y * 32;

    return (posX >= _boxLeft) && (posX < _boxRight) && (posY >= _boxTop) && (posY < _boxBottom);
}

void BuildingPlacer::computeResourceBox()
{
	auto self = _opponentView->self();
	if (self == nullptr)
	{
		return;
	}

    BWAPI::Position start(self->getStartLocation());
    std::vector<BWAPI::Unit> unitsAroundNexus;

    for (auto & unit : BWAPI::Broodwar->getAllUnits())
    {
        // if the units are less than 400 away add them if they are resources
        if (unit->getDistance(start) < 300 && unit->getType().isResourceContainer())
        {
            unitsAroundNexus.push_back(unit);
        }
    }

    for (auto & unit : unitsAroundNexus)
    {
        int x = unit->getPosition().x;
        int y = unit->getPosition().y;

        int left = x - unit->getType().dimensionLeft();
        int right = x + unit->getType().dimensionRight() + 1;
        int top = y - unit->getType().dimensionUp();
        int bottom = y + unit->getType().dimensionDown() + 1;

        _boxTop     = top < _boxTop       ? top    : _boxTop;
        _boxBottom  = bottom > _boxBottom ? bottom : _boxBottom;
        _boxLeft    = left < _boxLeft     ? left   : _boxLeft;
        _boxRight   = right > _boxRight   ? right  : _boxRight;
    }

    //_logger.log("%d %d %d %d", boxTop, boxBottom, boxLeft, boxRight);
}

// makes final checks to see if a building can be built at a certain location
BuildingPlaceCheckStatus BuildingPlacer::canBuildHere(BWAPI::TilePosition position,const Building & b) const
{
    //returns true if we can build this type of unit here. Takes into account reserved tiles.
    if (!BWAPI::Broodwar->canBuildHere(position,b.type,b.builderUnit))
    {
        //BWAPI::Broodwar->drawCircleMap(BWAPI::Position(position), 8, BWAPI::Colors::Red, true);
        return BuildingPlaceCheckStatus::CannotBuild;
    }

    // check the reserve map
    for (int x = position.x; x < position.x + b.type.tileWidth(); x++)
    {
        for (int y = position.y; y < position.y + b.type.tileHeight(); y++)
        {
            if (_reserveMap[x][y])
            {
                //BWAPI::Broodwar->drawCircleMap(BWAPI::Position(position), 8, BWAPI::Colors::Blue, true);
                return BuildingPlaceCheckStatus::LocationReserved;
            }
        }
    }

    // if it overlaps a base location return false
    if (tileOverlapsBaseLocation(position,b.type))
    {
        //BWAPI::Broodwar->drawCircleMap(BWAPI::Position(position), 8, BWAPI::Colors::Yellow, true);
        return BuildingPlaceCheckStatus::BaseLocationOverlap;
    }

    if (isInResourceBox(position.x,position.y))
    {
        //BWAPI::Broodwar->drawCircleMap(BWAPI::Position(position), 8, BWAPI::Colors::Orange, true);
        return BuildingPlaceCheckStatus::ResourceOverlap;
    }

    //BWAPI::Broodwar->drawCircleMap(BWAPI::Position(position), 8, BWAPI::Colors::Green, true);
    return BuildingPlaceCheckStatus::CanBuild;
}

bool BuildingPlacer::tileBlocksAddon(BWAPI::TilePosition position) const
{

    for (int i=0; i<=2; ++i)
    {
        for (auto & unit : BWAPI::Broodwar->getUnitsOnTile(position.x - i,position.y))
        {
            if (unit->getType() == BWAPI::UnitTypes::Terran_Command_Center ||
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
BuildingPlaceCheckStatus BuildingPlacer::canBuildHereWithSpace(BWAPI::TilePosition position,const Building & b,int buildDist,bool horizontalOnly) const
{
    BWAPI::UnitType type = b.type;

    //if we can't build here, we of course can't build here with space
	auto checkStatus = canBuildHere(position, b);
    if (checkStatus != BuildingPlaceCheckStatus::CanBuild)
    {
        return checkStatus;
    }

    // height and width of the building
    int width  = b.type.tileWidth();
    int height = b.type.tileHeight();

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

        BWAPI::TilePosition builderTile(position.x - builderType.tileWidth(),position.y + 2 - builderType.tileHeight());

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
    if (startx < 0 || starty < 0 || endx > _width || endx < position.x + width || endy > _height)
    {
        return BuildingPlaceCheckStatus::CannotBuild;
    }

    // if we can't build here, or space is reserved, or it's in the resource box, we can't build here
    for (int x = startx; x < endx; x++)
    {
        for (int y = starty; y < endy; y++)
        {
            if (!b.type.isRefinery())
            {
                if (!buildable(b,x,y))
                {
                    return BuildingPlaceCheckStatus::CannotBuild;
                }

				if (_reserveMap[x][y])
				{
					return BuildingPlaceCheckStatus::LocationReserved;
				}
            }
        }
    }

    return BuildingPlaceCheckStatus::CanBuild;
}

BWAPI::TilePosition BuildingPlacer::getBuildLocationNear(const Building & b,int buildDist,bool horizontalOnly) const
{
    SparCraft::Timer t;
    t.start();

    // get the precomputed vector of tile positions which are sorted closes to this location
    const std::vector<BWAPI::TilePosition> & closestToBuilding = _mapTools->getClosestTilesTo(BWAPI::Position(b.desiredPosition));

    // special easy case of having no pylons
    int numPylons = _opponentView->self()->completedUnitCount(BWAPI::UnitTypes::Protoss_Pylon);
    if (b.type.requiresPsi() && numPylons == 0)
    {
        return BWAPI::TilePositions::None;
    }

    // iterate through the list until we've found a suitable location
    for (size_t i(0); i < closestToBuilding.size(); ++i)
    {
		auto checkStatus = canBuildHereWithSpace(closestToBuilding[i], b, buildDist, horizontalOnly);
        if (checkStatus == BuildingPlaceCheckStatus::CanBuild)
        {
            return closestToBuilding[i];
        }
    }

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
    for (const BaseLocation* base : _bases->getBaseLocations())
    {
        // dimensions of the base location
        int bx1 = base->getDepotTilePosition().x;
        int by1 = base->getDepotTilePosition().y;
		auto resourceDepot = UnitUtil::getResourceDepot(_opponentView->self()->getRace());
        int bx2 = bx1 + resourceDepot.tileWidth();
        int by2 = by1 + resourceDepot.tileHeight();

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

bool BuildingPlacer::buildable(const Building & b,int x,int y) const
{
    BWAPI::TilePosition tp(x,y);

    //returns true if this tile is currently buildable, takes into account units on tile
    if (!tp.isValid() || !BWAPI::Broodwar->isBuildable(x,y, true))
    {
        return false;
    }

    if ((_opponentView->self()->getRace() == BWAPI::Races::Terran) && tileBlocksAddon(BWAPI::TilePosition(x,y)))
    {
        return false;
    }

    return true;
}

void BuildingPlacer::reserveTiles(BWAPI::TilePosition position,int width,int height)
{
    int rwidth = _reserveMap.size();
    int rheight = _reserveMap[0].size();
    for (int x = position.x; x < position.x + width && x < rwidth; x++)
    {
        for (int y = position.y; y < position.y + height && y < rheight; y++)
        {
            _reserveMap[x][y] = true;
        }
    }
}

void BuildingPlacer::freeTiles(BWAPI::TilePosition position, int width, int height)
{
    int rwidth = _reserveMap.size();
    int rheight = _reserveMap[0].size();

    for (int x = position.x; x < position.x + width && x < rwidth; x++)
    {
        for (int y = position.y; y < position.y + height && y < rheight; y++)
        {
            _reserveMap[x][y] = false;
        }
    }
}

BWAPI::TilePosition BuildingPlacer::getRefineryPosition()
{
    BWAPI::TilePosition closestGeyser = BWAPI::TilePositions::None;
    double minGeyserDistanceFromHome = std::numeric_limits<double>::max();
    BWAPI::Position homePosition = BWAPI::Position(_opponentView->self()->getStartLocation());

    // for each geyser
    for (auto & geyser : BWAPI::Broodwar->getStaticGeysers())
    {
        if (geyser->getType() != BWAPI::UnitTypes::Resource_Vespene_Geyser)
        {
            continue;
        }

        BWAPI::Position geyserPos = geyser->getInitialPosition();
        BWAPI::TilePosition geyserTilePos = geyser->getInitialTilePosition();

        // check to see if it's next to one of our depots
        bool nearDepot = false;
        for (auto & unit : _opponentView->self()->getUnits())
        {
            if (unit->getType().isResourceDepot() && unit->getDistance(geyserPos) < 300)
            {
                nearDepot = true;
            }
        }

        if (nearDepot)
        {
            double homeDistance = geyser->getDistance(homePosition);

            if (homeDistance < minGeyserDistanceFromHome)
            {
                minGeyserDistanceFromHome = homeDistance;
                closestGeyser = geyser->getInitialTilePosition();
            }
        }
    }
    
    return closestGeyser;
}

bool BuildingPlacer::isReserved(int x, int y) const
{
    int rwidth = _reserveMap.size();
    int rheight = _reserveMap[0].size();
    if (x < 0 || y < 0 || x >= rwidth || y >= rheight)
    {
        return false;
    }

    return _reserveMap[x][y];
}


int BuildingPlacer::reserveWidth() const
{
	int rwidth = _reserveMap.size();
	return rwidth;
}

int BuildingPlacer::reserveHeight() const
{
	int rheight = _reserveMap[0].size();
	return rheight;
}
