
#include "Common.h"
#include "BaseLocation.h"
#include "MapTools.h"
#include "Global.h"
#include <sstream>
#include <iostream>

using namespace UAlbertaBot;

const int NearBaseLocationTileDistance = 20;

BaseLocation::BaseLocation(int baseID, const std::vector<BWAPI::Unit> & resources)
    : m_baseID(baseID)
{
    PROFILE_FUNCTION();

    m_isPlayerStartLocation[BWAPI::Broodwar->self()]  = false;
    m_isPlayerStartLocation[BWAPI::Broodwar->enemy()] = false;
    m_isPlayerOccupying[BWAPI::Broodwar->self()]      = false;
    m_isPlayerOccupying[BWAPI::Broodwar->enemy()]     = false;

    int resourceCenterX = 0;
    int resourceCenterY = 0;

    // add each of the resources to its corresponding container
    for (auto & resource : resources)
    {
        if (resource->getType().isMineralField())
        {
            m_minerals.push_back(resource);
            m_mineralPositions.push_back(resource->getPosition());

            // add the position of the minerals to the center
            resourceCenterX += resource->getPosition().x;
            resourceCenterY += resource->getPosition().y;
        }
        else
        {
            m_geysers.push_back(resource);
            m_geyserPositions.push_back(resource->getPosition());

            // pull the resource center toward the geyser if it exists
            resourceCenterX += resource->getPosition().x;
            resourceCenterY += resource->getPosition().y;
        }

        // set the limits of the base location bounding box
        const int resWidth = 32;
        const int resHeight = 32;

        m_left   = std::min(m_left, resource->getPosition().x - resWidth);
        m_right  = std::max(m_right, resource->getPosition().x + resWidth);
        m_top    = std::max(m_top, resource->getPosition().y + resHeight);
        m_bottom = std::min(m_bottom, resource->getPosition().y - resHeight);
    }

    // calculate the center of the resources
    const size_t numResources = m_minerals.size() + m_geysers.size();

    m_centerOfResources = BWAPI::Position(m_left + (m_right-m_left)/2, m_top + (m_bottom-m_top)/2);

    // compute this BaseLocation's DistanceMap, which will compute the ground distance
    // from the center of its recourses to every other tile on the map
    m_distanceMap = DistanceMap();
    m_distanceMap.computeDistanceMap(BWAPI::TilePosition(m_centerOfResources));

    // check to see if this is a start location for the map
    for (auto & startTilePos : BWAPI::Broodwar->getStartLocations())
    {
        auto groundDistance = getGroundDistance(startTilePos);

        if (containsPosition(BWAPI::Position(startTilePos)))
        {
            m_isStartLocation = true;
            m_depotPosition = BWAPI::TilePosition(startTilePos);
            m_startPosition = startTilePos;
            break;
        }
    }

    // if this base location position is near our own resource depot, it's our start location
    for (auto & unit : BWAPI::Broodwar->getAllUnits())
    {
        if (unit->getPlayer() == BWAPI::Broodwar->self() && unit->getType().isResourceDepot())
        {
            if (containsPosition(unit->getPosition()))
            {
                m_isPlayerStartLocation[BWAPI::Broodwar->self()] = true;
                m_isStartLocation = true;
                m_isPlayerOccupying[BWAPI::Broodwar->self()] = true;
                break;
            }
        }
    }

    // if it's not a start location, we need to calculate the depot position
    if (!isStartLocation())
    {
        const BWAPI::UnitType depot = BWAPI::Broodwar->self()->getRace().getResourceDepot();

        const int offsetX = 1;
        const int offsetY = 1;

        // the position of the depot will be the closest spot we can build one from the resource center
        for (auto & tile : getClosestTiles())
        {
            // the build position will be up-left of where this tile is
            // this means we are positioning the center of the resouce depot
            const BWAPI::TilePosition buildTile(tile.x - offsetX, tile.y - offsetY);

            if (BWAPI::Broodwar->canBuildHere(buildTile, depot))
            {
                m_depotPosition = buildTile;
                break;
            }
        }
    }
}

// TODO: calculate the actual depot position
const BWAPI::TilePosition & BaseLocation::getDepotPosition() const
{
    return m_depotPosition;
}

void BaseLocation::setPlayerOccupying(BWAPI::Player player, bool occupying)
{
    m_isPlayerOccupying[player] = occupying;

    // if this base is a start location that's occupied by the enemy, it's that enemy's start location
    if (occupying && player == BWAPI::Broodwar->enemy() && isStartLocation() && m_isPlayerStartLocation[player] == false)
    {
        m_isPlayerStartLocation[player] = true;
    }
}

bool BaseLocation::isInResourceBox(int tileX, int tileY) const
{
    const int px = tileX * 32;
    const int py = tileY * 32;
    return px >= m_left && px < m_right && py < m_top && py >= m_bottom;
}

bool BaseLocation::isOccupiedByPlayer(BWAPI::Player player) const
{
    return m_isPlayerOccupying.at(player);
}

bool BaseLocation::isExplored() const
{
    return BWAPI::Broodwar->isExplored(BWAPI::TilePosition(m_centerOfResources));
}

bool BaseLocation::isPlayerStartLocation(BWAPI::Player player) const
{
    return m_isPlayerStartLocation.at(player);
}

bool BaseLocation::isConnected(const BWAPI::Position& pos) const
{
    return getGroundDistance(pos) >= 0;
}

bool BaseLocation::containsPosition(const BWAPI::Position & pos) const
{
    if (!pos.isValid() || (pos.x == 0 && pos.y == 0) || !isConnected(pos))
    {
        return false;
    }

    return (getGroundDistance(pos) < NearBaseLocationTileDistance);
}

const std::vector<BWAPI::Unit> & BaseLocation::getGeysers() const
{
    return m_geysers;
}

const std::vector<BWAPI::Unit> & BaseLocation::getMinerals() const
{
    return m_minerals;
}

const BWAPI::Position & BaseLocation::getPosition() const
{
    return m_centerOfResources;
}

int BaseLocation::getGroundDistance(const BWAPI::Position & pos) const
{
    return m_distanceMap.getDistance(pos);
}

int BaseLocation::getGroundDistance(const BWAPI::TilePosition & pos) const
{
    return m_distanceMap.getDistance(pos);
}

bool BaseLocation::isStartLocation() const
{
    return m_isStartLocation;
}

const std::vector<BWAPI::TilePosition> & BaseLocation::getClosestTiles() const
{
    return m_distanceMap.getSortedTiles();
}

void BaseLocation::draw()
{
    int radius = 16;

    BWAPI::Broodwar->drawCircleMap(m_centerOfResources, 16, BWAPI::Color(255, 255, 0), true);

    if (m_startPosition.x != 0)
    {
        BWAPI::Broodwar->drawLineMap(m_centerOfResources, BWAPI::Position(m_startPosition), BWAPI::Colors::Red);
    }

    std::stringstream ss;
    ss << "BaseLocation: " << m_baseID << "\n";
    ss << "Start Loc:    " << (isStartLocation() ? "true" : "false") << "\n";
    ss << "Minerals:     " << m_mineralPositions.size() << "\n";
    ss << "Geysers:      " << m_geyserPositions.size() << "\n";
    ss << "Occupied By:  ";

    if (isOccupiedByPlayer(BWAPI::Broodwar->self()))
    {
        ss << "Self ";
    }

    if (isOccupiedByPlayer(BWAPI::Broodwar->enemy()))
    {
        ss << "Enemy ";
    }


    BWAPI::Broodwar->drawTextMap(BWAPI::Position(m_left, m_top + 3), ss.str().c_str());
    BWAPI::Broodwar->drawTextMap(BWAPI::Position(m_left, m_bottom), ss.str().c_str());

    // draw the base bounding box
    BWAPI::Broodwar->drawLineMap(m_left, m_top, m_right, m_top, BWAPI::Colors::White);
    BWAPI::Broodwar->drawLineMap(m_right, m_top, m_right, m_bottom, BWAPI::Colors::White);
    BWAPI::Broodwar->drawLineMap(m_right, m_bottom, m_left, m_bottom, BWAPI::Colors::White);
    BWAPI::Broodwar->drawLineMap(m_left, m_bottom, m_left, m_top, BWAPI::Colors::White);

    for (auto & mineralPos : m_mineralPositions)
    {
        const BWAPI::TilePosition mineralTile(mineralPos);
        Global::Map().drawTile(mineralTile.x,   mineralTile.y, BWAPI::Color(0, 255, 255));
        Global::Map().drawTile(mineralTile.x-1, mineralTile.y, BWAPI::Color(0, 255, 255));
    }

    for (auto & geyserPos : m_geyserPositions)
    {
        const BWAPI::TilePosition geyserTile(geyserPos);
        Global::Map().drawTile(geyserTile.x,   geyserTile.y,   BWAPI::Color(0, 255, 0));
        Global::Map().drawTile(geyserTile.x+1, geyserTile.y,   BWAPI::Color(0, 255, 0));
        Global::Map().drawTile(geyserTile.x-1, geyserTile.y,   BWAPI::Color(0, 255, 0));
        Global::Map().drawTile(geyserTile.x-2, geyserTile.y,   BWAPI::Color(0, 255, 0));
        Global::Map().drawTile(geyserTile.x,   geyserTile.y-1, BWAPI::Color(0, 255, 0));
        Global::Map().drawTile(geyserTile.x+1, geyserTile.y-1, BWAPI::Color(0, 255, 0));
        Global::Map().drawTile(geyserTile.x-1, geyserTile.y-1, BWAPI::Color(0, 255, 0));
        Global::Map().drawTile(geyserTile.x-2, geyserTile.y-1, BWAPI::Color(0, 255, 0));

        BWAPI::Broodwar->drawCircleMap(geyserPos, radius, BWAPI::Color(0, 255, 0), true);
    }

    if (m_isStartLocation)
    {
        BWAPI::Broodwar->drawCircleMap(BWAPI::Position(m_depotPosition), radius, BWAPI::Color(255, 0, 0));
    }

    BWAPI::Broodwar->drawBoxMap(m_depotPosition.x, m_depotPosition.y, m_depotPosition.x+32, m_depotPosition.y+32, BWAPI::Color(0, 0, 255));

    //m_distanceMap.draw();
}

bool BaseLocation::isMineralOnly() const
{
    return getGeysers().empty();
}