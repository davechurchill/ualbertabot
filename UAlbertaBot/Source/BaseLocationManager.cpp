#include "BaseLocationManager.h"
#include "Global.h"
#include "UnitUtil.h"

using namespace UAlbertaBot;

BaseLocationManager::BaseLocationManager()
    : _tileBaseLocations(BWAPI::Broodwar->mapWidth(), std::vector<BaseLocation *>(BWAPI::Broodwar->mapHeight(), nullptr))
{
    _playerStartingBaseLocations[BWAPI::Broodwar->self()]  = nullptr;
    _playerStartingBaseLocations[BWAPI::Broodwar->enemy()] = nullptr;
}

void BaseLocationManager::onStart()
{
    // a BaseLocation will be anything where there are minerals to mine
    // so we will first look over all minerals and cluster them based on some distance
    const int clusterDistance = 20;

    // stores each cluster of resources based on some ground distance
    std::vector<std::vector<BWAPI::Unit>> resourceClusters;
    for (auto & mineral : BWAPI::Broodwar->getStaticMinerals())
    {
        // skip minerals that don't have more than 100 starting minerals
        // these are probably stupid map-blocking minerals to confuse us
        if (mineral->getResources() <= 100)
        {
            continue;
        }

        bool foundCluster = false;
        for (std::vector<BWAPI::Unit> & cluster : resourceClusters)
        {
            int groundDist = Global::Map().getGroundDistance(mineral->getPosition(), UnitUtil::GetUnitsetCenter(cluster));

            if (groundDist >= 0 && groundDist < clusterDistance)
            {
                cluster.push_back(mineral);
                foundCluster = true;
                break;
            }
        }

        if (!foundCluster)
        {
            resourceClusters.push_back(std::vector<BWAPI::Unit>());
            resourceClusters.back().push_back(mineral);
        }
    }

    // add geysers only to existing resource clusters
    for (auto & geyser : BWAPI::Broodwar->getStaticGeysers())
    {
        if (geyser->getResources() <= 100)
        {
            continue;
        }

        for (std::vector<BWAPI::Unit> & cluster : resourceClusters)
        {
            int groundDist = Global::Map().getGroundDistance(geyser->getPosition(), UnitUtil::GetUnitsetCenter(cluster));

            if (groundDist >= 0 && groundDist < clusterDistance)
            {
                cluster.push_back(geyser);
                break;
            }
        }
    }

    // add the base locations if there are more than 4 resouces in the cluster
    int baseID = 0;
    for (auto & cluster : resourceClusters)
    {
        if (cluster.size() > 4)
        {
            _baseLocationData.push_back(BaseLocation(baseID++, cluster));
        }
    }

    // construct the vectors of base location pointers, this is safe since they will never change
    for (auto & baseLocation : _baseLocationData)
    {
        _baseLocationPtrs.push_back(&baseLocation);

        // if it's a start location, add it to the start locations
        if (baseLocation.isStartLocation())
        {
            _startingBaseLocations.push_back(&baseLocation);
        }

        // if it's our starting location, set the pointer
        if (baseLocation.isPlayerStartLocation(BWAPI::Broodwar->self()))
        {
            _playerStartingBaseLocations[BWAPI::Broodwar->self()] = &baseLocation;
        }

        if (baseLocation.isPlayerStartLocation(BWAPI::Broodwar->enemy()))
        {
            _playerStartingBaseLocations[BWAPI::Broodwar->enemy()] = &baseLocation;
        }
    }

    // construct the map of tile positions to base locations
    for (int x=0; x < BWAPI::Broodwar->mapWidth(); ++x)
    {
        for (int y=0; y < BWAPI::Broodwar->mapHeight(); ++y)
        {
            for (auto & baseLocation : _baseLocationData)
            {
                BWAPI::Position pos(x*32 + 16, y*32 + 16);

                if (baseLocation.containsPosition(pos))
                {
                    _tileBaseLocations[x][y] = &baseLocation;
                    
                    break;
                }
            }
        }
    }

    // construct the sets of occupied base locations
    _occupiedBaseLocations[BWAPI::Broodwar->self()] = std::set<const BaseLocation *>();
    _occupiedBaseLocations[BWAPI::Broodwar->enemy()] = std::set<const BaseLocation *>();

    // check to see that we have set a base location for ourself
    UAB_ASSERT(_playerStartingBaseLocations[BWAPI::Broodwar->self()] != nullptr, "We didn't set a valid selfStartLocation in BaseLocations");
}

void BaseLocationManager::update()
{   
    // reset the player occupation information for each location
    for (auto & baseLocation : _baseLocationData)
    {
        baseLocation.setPlayerOccupying(BWAPI::Broodwar->self(), false);
        baseLocation.setPlayerOccupying(BWAPI::Broodwar->enemy(), false);
    }

    // for each unit on the map, update which base location it may be occupying
    for (auto & unit : BWAPI::Broodwar->getAllUnits())
    {
        // we only care about buildings on the ground
        if (!unit->getType().isBuilding() || unit->isLifted() || unit->getPlayer()->isNeutral())
        {
            continue;
        }

        BaseLocation * baseLocation = _getBaseLocation(unit->getPosition());

        if (baseLocation != nullptr)
        {
            baseLocation->setPlayerOccupying(unit->getPlayer(), true);
        }
    }

    // update the starting locations of the enemy player
    if (_playerStartingBaseLocations[BWAPI::Broodwar->enemy()] == nullptr)
    {
        for (auto & baseLocation : _baseLocationData)
        {
            if (baseLocation.isPlayerStartLocation(BWAPI::Broodwar->enemy()))
            {
                _playerStartingBaseLocations[BWAPI::Broodwar->enemy()] = &baseLocation;
            }
        }
    }

    // update the occupied base locations for each player
    _occupiedBaseLocations[BWAPI::Broodwar->self()] = std::set<const BaseLocation *>();
    _occupiedBaseLocations[BWAPI::Broodwar->enemy()] = std::set<const BaseLocation *>();
    for (auto & baseLocation : _baseLocationData)
    {
        if (baseLocation.isOccupiedByPlayer(BWAPI::Broodwar->self()))
        {
            _occupiedBaseLocations[BWAPI::Broodwar->self()].insert(&baseLocation);
        }

        if (baseLocation.isOccupiedByPlayer(BWAPI::Broodwar->enemy()))
        {
            _occupiedBaseLocations[BWAPI::Broodwar->enemy()].insert(&baseLocation);
        }
    }

    // draw the debug information for each base location
    drawBaseLocations();
}

void BaseLocationManager::drawBaseLocations()
{
    for (auto & baseLocation : _baseLocationData)
    {
        baseLocation.draw();
    }
}

const std::vector<const BaseLocation *> & BaseLocationManager::getBaseLocations() const
{
    return _baseLocationPtrs;
}

const std::vector<const BaseLocation *> & BaseLocationManager::getStartingBaseLocations() const
{
    return _startingBaseLocations;
}

const BaseLocation * BaseLocationManager::getPlayerStartingBaseLocation(BWAPI::Player player)
{
    return _playerStartingBaseLocations[player];
}

const std::set<const BaseLocation *> & BaseLocationManager::getOccupiedBaseLocations(BWAPI::Player player)
{
    return _occupiedBaseLocations[player];
}

const BaseLocation * BaseLocationManager::getBaseLocation(BWAPI::TilePosition pos) const
{
    if (!pos.isValid())
    {
        return nullptr;
    }

    return _tileBaseLocations[pos.x][pos.y];
}

BaseLocation * BaseLocationManager::_getBaseLocation(BWAPI::TilePosition pos) const
{
    if (!pos.isValid())
    {
        return nullptr;
    }

    return _tileBaseLocations[pos.x][pos.y];
}

BaseLocation * BaseLocationManager::_getBaseLocation(BWAPI::Position pos) const
{
    if (!pos.isValid())
    {
        return nullptr;
    }

    return _getBaseLocation(BWAPI::TilePosition(pos));
}

const BaseLocation * BaseLocationManager::getBaseLocation(BWAPI::Position pos) const
{
    if (!pos.isValid())
    {
        return nullptr;
    }

    return getBaseLocation(BWAPI::TilePosition(pos));
}

BWAPI::TilePosition BaseLocationManager::getNextExpansion(BWAPI::Player player)
{
    const BaseLocation * closestBase = nullptr;
    int minDistance = std::numeric_limits<int>::max();

    BWAPI::TilePosition homeTile = player->getStartLocation();
    
    // for each base location
    for (const BaseLocation * base : getBaseLocations())
    {
        // skip mineral only and starting locations (TODO: fix this)
        if (base->isMineralOnly() || base->isStartLocation())
        {
            continue;
        }

        // get the tile position of the base
        BWAPI::TilePosition tile = base->getTilePosition();
        bool buildingInTheWay = false;

        for (int x = 0; x < BWAPI::Broodwar->self()->getRace().getResourceDepot().tileWidth(); ++x)
        {
            for (int y = 0; y < BWAPI::Broodwar->self()->getRace().getResourceDepot().tileHeight(); ++y)
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
        double distanceFromHome = Global::Map().getGroundDistance(thisTile, myBasePosition);

        // if it is not connected, continue
        if (Global::Map().isConnected(BWAPI::Position(tile), BWAPI::Position(homeTile)) || distanceFromHome < 0)
        {
            continue;
        }

        if (!closestBase || distanceFromHome < minDistance)
        {
            closestBase = base;
            minDistance = distanceFromHome;
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
