
#include "Common.h"
#include "BaseLocationManager.h"
#include "InformationManager.h"
#include "MapTools.h"
#include "Global.h"
#include "UnitData.h"

using namespace UAlbertaBot;

BaseLocationManager::BaseLocationManager()
{
    onStart();
}

BWAPI::Position BaseLocationManager::calcCenter(const std::vector<BWAPI::Unit> & units)
{
    if (units.empty())
    {
        return BWAPI::Position(0, 0);
    }

    int cx = 0;
    int cy = 0;

    for (auto & unit : units)
    {
        cx += unit->getPosition().x;
        cy += unit->getPosition().y;
    }

    return BWAPI::Position(cx / units.size(), cy / units.size());
}

void BaseLocationManager::onStart()
{
    PROFILE_FUNCTION();

    m_tileBaseLocations = std::vector<std::vector<BaseLocation *>>(BWAPI::Broodwar->mapWidth(), std::vector<BaseLocation *>(BWAPI::Broodwar->mapHeight(), nullptr));
    m_playerStartingBaseLocations[BWAPI::Broodwar->self()]  = nullptr;
    m_playerStartingBaseLocations[BWAPI::Broodwar->enemy()] = nullptr;

    // Use StarDraft to find the borders of the bases on the map
    m_baseBorders = BaseBorderFinder(Global::Map().getStarDraftMap());
    for (size_t baseID = 0; baseID < m_baseBorders.getBaseBorders().size(); baseID++)
    {
        const auto & border = m_baseBorders.getBaseBorders()[baseID];

        // fill a vector with all the resource units in the border
        std::vector<BWAPI::Unit> resources;

        // for each static unit on the map
        for (auto unit : BWAPI::Broodwar->getStaticNeutralUnits())
        {
            // if it's a resource
            if (unit->getType().isMineralField() || unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser)
            {
                BWAPI::TilePosition tile(unit->getPosition());

                // if it's inside the border, add it to the vector
                if (border.contains(tile.x, tile.y))
                {
                    resources.push_back(unit);
                }
            }
        }

        // add a baselocation containing these resources
        m_baseLocationData.push_back(BaseLocation(baseID, resources));
    }

    // construct the vectors of base location pointers, this is safe since they will never change
    for (auto & baseLocation : m_baseLocationData)
    {
        m_baseLocationPtrs.push_back(&baseLocation);

        // if it's a start location, add it to the start locations
        if (baseLocation.isStartLocation())
        {
            m_startingBaseLocations.push_back(&baseLocation);
        }

        // if it's our starting location, set the pointer
        if (baseLocation.isPlayerStartLocation(BWAPI::Broodwar->self()))
        {
            m_playerStartingBaseLocations[BWAPI::Broodwar->self()] = &baseLocation;
        }

        if (baseLocation.isPlayerStartLocation(BWAPI::Broodwar->enemy()))
        {
            m_playerStartingBaseLocations[BWAPI::Broodwar->enemy()] = &baseLocation;
        }
    }

    // construct the map of tile positions to base locations
    for (int x=0; x < BWAPI::Broodwar->mapWidth(); ++x)
    {
        for (int y = 0; y < BWAPI::Broodwar->mapHeight(); ++y)
        {
            for (auto & baseLocation : m_baseLocationData)
            {
                BWAPI::Position pos(BWAPI::TilePosition(x, y));

                if (baseLocation.containsPosition(pos))
                {
                    m_tileBaseLocations[x][y] = &baseLocation;

                    break;
                }
            }
        }
    }

    // construct the sets of occupied base locations
    m_occupiedBaseLocations[BWAPI::Broodwar->self()] = std::set<const BaseLocation *>();
    m_occupiedBaseLocations[BWAPI::Broodwar->enemy()] = std::set<const BaseLocation *>();
}

void BaseLocationManager::onFrame()
{
    PROFILE_FUNCTION();

    drawBaseLocations();

    // reset the player occupation information for each location
    for (auto & baseLocation : m_baseLocationData)
    {
        baseLocation.setPlayerOccupying(BWAPI::Broodwar->self(), false);
        baseLocation.setPlayerOccupying(BWAPI::Broodwar->self(), false);
    }

    // for each unit on the map, update which base location it may be occupying
    for (auto & unit : BWAPI::Broodwar->self()->getUnits())
    {
        // we only care about buildings on the ground
        if (!unit->getType().isBuilding() || unit->isFlying())
        {
            continue;
        }

        BaseLocation * baseLocation = getBaseLocation(unit->getPosition());

        if (baseLocation != nullptr)
        {
            baseLocation->setPlayerOccupying(unit->getPlayer(), true);
        }
    }

    // update enemy base occupations
    for (const auto & kv : Global::Info().getUnitInfo(BWAPI::Broodwar->enemy()))
    {
        const UnitInfo & ui = kv.second;

        if (ui.type.isBuilding())
        {
            continue;
        }

        BaseLocation * baseLocation = getBaseLocation(ui.lastPosition);

        if (baseLocation != nullptr)
        {
            baseLocation->setPlayerOccupying(BWAPI::Broodwar->enemy(), true);
        }
    }

    // update the starting locations of the enemy player
    // this will happen one of two ways:

    // 1. we've seen the enemy base directly, so the baselocation will know
    if (m_playerStartingBaseLocations[BWAPI::Broodwar->enemy()] == nullptr)
    {
        for (auto & baseLocation : m_baseLocationData)
        {
            if (baseLocation.isPlayerStartLocation(BWAPI::Broodwar->enemy()))
            {
                m_playerStartingBaseLocations[BWAPI::Broodwar->enemy()] = &baseLocation;
            }
        }
    }

    // 2. we've explored every other start location and haven't seen the enemy yet
    if (m_playerStartingBaseLocations[BWAPI::Broodwar->enemy()] == nullptr)
    {
        const int numStartLocations = (int)getStartingBaseLocations().size();
        int numExploredLocations = 0;
        BaseLocation * unexplored = nullptr;

        for (auto & baseLocation : m_baseLocationData)
        {
            if (!baseLocation.isStartLocation())
            {
                continue;
            }

            if (baseLocation.isExplored())
            {
                numExploredLocations++;
            }
            else
            {
                unexplored = &baseLocation;
            }
        }

        // if we have explored all but one location, then the unexplored one is the enemy start location
        if (numExploredLocations == numStartLocations - 1 && unexplored != nullptr)
        {
            m_playerStartingBaseLocations[BWAPI::Broodwar->enemy()] = unexplored;
            unexplored->setPlayerOccupying(BWAPI::Broodwar->enemy(), true);
        }
    }

    // update the occupied base locations for each player
    m_occupiedBaseLocations[BWAPI::Broodwar->self()] = std::set<const BaseLocation *>();
    m_occupiedBaseLocations[BWAPI::Broodwar->enemy()] = std::set<const BaseLocation *>();
    for (auto & baseLocation : m_baseLocationData)
    {
        if (baseLocation.isOccupiedByPlayer(BWAPI::Broodwar->self()))
        {
            m_occupiedBaseLocations[BWAPI::Broodwar->self()].insert(&baseLocation);
        }

        if (baseLocation.isOccupiedByPlayer(BWAPI::Broodwar->enemy()))
        {
            m_occupiedBaseLocations[BWAPI::Broodwar->enemy()].insert(&baseLocation);
        }
    }

    // sanity check: make sure we have as many starting locations as BWAPI says
    if (getStartingBaseLocations().size() != BWAPI::Broodwar->getStartLocations().size())
    {
        std::cout << "\nWARNING: BaseLocationManager start location mismatch: " << BWAPI::Broodwar->mapFileName() << "\n";
        std::cout << "         BaseLocationManager found " << getStartingBaseLocations().size() << " starting locations\n";
        std::cout << "         BWAPI says that there are " << BWAPI::Broodwar->getStartLocations().size() << " starting locations\n\n";

        for (auto tp : BWAPI::Broodwar->getStartLocations())
        {
            BWAPI::Broodwar->drawCircleMap(BWAPI::Position(tp), 64, BWAPI::Colors::Red, true);
        }
    }
}

BaseLocation * BaseLocationManager::getBaseLocation(const BWAPI::Position & pos) const
{
    if (!pos.isValid()) { return nullptr; }

    return m_tileBaseLocations[pos.x / 32][pos.y / 32];
}

void BaseLocationManager::drawBaseLocations()
{
    for (auto & baseLocation : m_baseLocationData)
    {
        baseLocation.draw();
    }

    // draw a purple sphere at the next expansion location
    //BWAPI::TilePosition nextExpansionPosition = getNextExpansion(BWAPI::Broodwar->self());

    //BWAPI::Broodwar->drawCircleMap(BWAPI::Position(nextExpansionPosition), 32, BWAPI::Color(255, 0, 255), true);
    //BWAPI::Broodwar->drawTextMap(BWAPI::Position(nextExpansionPosition), "Next Expansion Location", BWAPI::Color(255, 0, 255));
}

const std::vector<const BaseLocation *> & BaseLocationManager::getBaseLocations() const
{
    return m_baseLocationPtrs;
}

const std::vector<const BaseLocation *> & BaseLocationManager::getStartingBaseLocations() const
{
    return m_startingBaseLocations;
}

const BaseLocation * BaseLocationManager::getPlayerStartingBaseLocation(BWAPI::Player player) const
{
    return m_playerStartingBaseLocations.at(player);
}

const std::set<const BaseLocation *> & BaseLocationManager::getOccupiedBaseLocations(BWAPI::Player player) const
{
    return m_occupiedBaseLocations.at(player);
}


BWAPI::TilePosition BaseLocationManager::getNextExpansion(BWAPI::Player player) const
{
    PROFILE_FUNCTION();

    const BaseLocation * homeBase = getPlayerStartingBaseLocation(player);
    const BaseLocation * closestBase = nullptr;
    int minDistance = std::numeric_limits<int>::max();

    BWAPI::TilePosition homeTile(homeBase->getPosition());

    for (auto & base : getBaseLocations())
    {
        // skip mineral only and starting locations (TODO: fix this)
        if (base->isMineralOnly() || base->isStartLocation())
        {
            continue;
        }

        // get the tile position of the base
        BWAPI::TilePosition tile = base->getDepotPosition();

        bool buildingInTheWay = false; // TODO: check if there are any units on the tile

        if (buildingInTheWay)
        {
            continue;
        }

        // the base's distance from our main nexus
        int distanceFromHome = homeBase->getGroundDistance(tile);

        // if it is not connected, continue
        if (distanceFromHome < 0)
        {
            continue;
        }

        if (!closestBase || distanceFromHome < minDistance)
        {
            closestBase = base;
            minDistance = distanceFromHome;
        }
    }

    return closestBase ? closestBase->getDepotPosition() : BWAPI::TilePosition(0, 0);
}
