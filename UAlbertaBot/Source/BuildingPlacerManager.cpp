#include "Common.h"
#include "BuildingPlacerManager.h"
#include "BaseLocationManager.h"
#include "Global.h"
#include "BuildingData.h"
#include "Global.h"
#include "MapTools.h"

using namespace UAlbertaBot;

BuildingPlacerManager::BuildingPlacerManager()
{
    m_reserveMap = Grid<int>(BWAPI::Broodwar->mapWidth(), BWAPI::Broodwar->mapHeight(), 0);

    computeResourceBox();
}

bool BuildingPlacerManager::isInResourceBox(int x, int y) const
{
    int posX(x * 32);
    int posY(y * 32);

    return (posX >= m_boxLeft) && (posX < m_boxRight) && (posY >= m_boxTop) && (posY < m_boxBottom);
}

void BuildingPlacerManager::computeResourceBox()
{
    PROFILE_FUNCTION();

    BWAPI::Position start(BWAPI::Broodwar->self()->getStartLocation());
    BWAPI::Unitset unitsAroundNexus;

    for (auto & unit : BWAPI::Broodwar->getAllUnits())
    {
        // if the units are less than 400 away add them if they are resources
        if (unit->getDistance(start) < 300 && unit->getType().isMineralField())
        {
            unitsAroundNexus.insert(unit);
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

        m_boxTop     = top < m_boxTop       ? top    : m_boxTop;
        m_boxBottom  = bottom > m_boxBottom ? bottom : m_boxBottom;
        m_boxLeft    = left < m_boxLeft     ? left   : m_boxLeft;
        m_boxRight   = right > m_boxRight   ? right  : m_boxRight;
    }

    //BWAPI::Broodwar->printf("%d %d %d %d", boxTop, boxBottom, boxLeft, boxRight);
}

// makes final checks to see if a building can be built at a certain location
bool BuildingPlacerManager::canBuildHere(BWAPI::TilePosition position, const Building & b) const
{
    /*if (!b.type.isRefinery() && !Global::Info().tileContainsUnit(position))
    {
    return false;
    }*/

    //returns true if we can build this type of unit here. Takes into account reserved tiles.
    if (!BWAPI::Broodwar->canBuildHere(position, b.type, b.builderUnit))
    {
        return false;
    }

    // check the reserve map
    for (int x = position.x; x < position.x + b.type.tileWidth(); x++)
    {
        for (int y = position.y; y < position.y + b.type.tileHeight(); y++)
        {
            if (m_reserveMap.get(x, y) == 1)
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

bool BuildingPlacerManager::tileBlocksAddon(BWAPI::TilePosition position) const
{

    for (int i=0; i<=2; ++i)
    {
        for (auto & unit : BWAPI::Broodwar->getUnitsOnTile(position.x - i, position.y))
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
//space value is stored in this->buildDistance.
bool BuildingPlacerManager::canBuildHereWithSpace(BWAPI::TilePosition position, const Building & b, int buildDist, bool horizontalOnly) const
{
    PROFILE_FUNCTION();

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
    for (int x = startx; x < endx; x++)
    {
        for (int y = starty; y < endy; y++)
        {
            if (!b.type.isRefinery())
            {
                if (!buildable(b, x, y) || m_reserveMap.get(x, y) || ((b.type != BWAPI::UnitTypes::Protoss_Photon_Cannon) && isInResourceBox(x, y)))
                {
                    return false;
                }
            }
        }
    }

    return true;
}

BWAPI::TilePosition BuildingPlacerManager::GetBuildLocation(const Building & b, int padding) const
{
    return BWAPI::TilePosition(0, 0);
}

BWAPI::TilePosition BuildingPlacerManager::getBuildLocationNear(const Building & b, int buildDist, bool horizontalOnly) const
{
    PROFILE_FUNCTION();

    // get the precomputed vector of tile positions which are sorted closes to this location
    const std::vector<BWAPI::TilePosition> & closestToBuilding = Global::Map().getClosestTilesTo(b.desiredPosition);

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
            //BWAPI::Broodwar->printf("Building Placer Took %d iterations, lasting %lf ms @ %lf iterations/ms, %lf setup ms", i, ms, (i / ms), ms1);

            return closestToBuilding[i];
        }
    }

    return  BWAPI::TilePositions::None;
}

bool BuildingPlacerManager::tileOverlapsBaseLocation(BWAPI::TilePosition tile, BWAPI::UnitType type) const
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
    for (auto base : Global::Bases().getBaseLocations())
    {
        // dimensions of the base location
        int bx1 = base->getDepotPosition().x;
        int by1 = base->getDepotPosition().y;
        int bx2 = bx1 + BWAPI::Broodwar->self()->getRace().getResourceDepot().tileWidth();
        int by2 = by1 + BWAPI::Broodwar->self()->getRace().getResourceDepot().tileHeight();

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

bool BuildingPlacerManager::buildable(const Building & b, int x, int y) const
{
    BWAPI::TilePosition tp(x, y);

    //returns true if this tile is currently buildable, takes into account units on tile
    if (!BWAPI::Broodwar->isBuildable(x, y))
    {
        return false;
    }

    if ((BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran) && tileBlocksAddon(BWAPI::TilePosition(x, y)))
    {
        return false;
    }

    for (auto & unit : BWAPI::Broodwar->getUnitsOnTile(x, y))
    {
        if ((b.builderUnit != nullptr) && (unit != b.builderUnit))
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

void BuildingPlacerManager::reserveTiles(BWAPI::TilePosition position, int width, int height)
{
    for (int x = position.x; x < position.x + width && x < (int)m_reserveMap.width(); x++)
    {
        for (int y = position.y; y < position.y + height && y < (int)m_reserveMap.height(); y++)
        {
            m_reserveMap.set(x, y, 1);
        }
    }
}

void BuildingPlacerManager::drawReservedTiles()
{
    if (!Config::Debug::DrawReservedBuildingTiles)
    {
        return;
    }

    PROFILE_FUNCTION();

    for (int x = 0; x < (int)m_reserveMap.width(); ++x)
    {
        for (int y = 0; y < (int)m_reserveMap.height(); ++y)
        {
            if (m_reserveMap.get(x, y) || isInResourceBox(x, y))
            {
                int x1 = x*32 + 8;
                int y1 = y*32 + 8;
                int x2 = (x+1)*32 - 8;
                int y2 = (y+1)*32 - 8;

                BWAPI::Broodwar->drawBoxMap(x1, y1, x2, y2, BWAPI::Colors::Yellow, false);
            }
        }
    }
}

void BuildingPlacerManager::freeTiles(BWAPI::TilePosition position, int width, int height)
{
    for (int x = position.x; x < position.x + width && x < (int)m_reserveMap.width(); x++)
    {
        for (int y = position.y; y < position.y + height && y < (int)m_reserveMap.height(); y++)
        {
            m_reserveMap.set(x, y, 0);
        }
    }
}

BWAPI::TilePosition BuildingPlacerManager::getRefineryPosition()
{
    BWAPI::TilePosition closestGeyser = BWAPI::TilePositions::None;
    double minGeyserDistanceFromHome = std::numeric_limits<double>::max();
    BWAPI::Position homePosition = BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation());

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
        for (auto & unit : BWAPI::Broodwar->self()->getUnits())
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

bool BuildingPlacerManager::isReserved(int x, int y) const
{
    if (x < 0 || y < 0 || x >= (int)m_reserveMap.width() || y >= (int)m_reserveMap.height())
    {
        return false;
    }

    return m_reserveMap.get(x, y);
}

