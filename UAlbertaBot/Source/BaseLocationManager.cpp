#include "BaseLocationManager.h"
#include "Global.h"
#include "UnitUtil.h"

using namespace UAlbertaBot;

BaseLocationManager::BaseLocationManager()
{
    _playerStartingBaseLocations[BWAPI::Broodwar->self()]  = nullptr;
    _playerStartingBaseLocations[BWAPI::Broodwar->enemy()] = nullptr;
}

void BaseLocationManager::onStart()
{
    // a BaseLocation will be anything where there are minerals to mine
    // so we will first look over all minerals and cluster them based on some distance
    const int clusterDistance = 300;

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
            if (mineral->getPosition().getDistance(UnitUtil::GetUnitsetCenter(cluster)) < clusterDistance)
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
            if (geyser->getPosition().getDistance(UnitUtil::GetUnitsetCenter(cluster)) < clusterDistance)
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
        if (baseLocation.getTilePosition() == BWAPI::Broodwar->self()->getStartLocation())
        {
            _playerStartingBaseLocations[BWAPI::Broodwar->self()] = &baseLocation;
        }
    }

    // check to see that we have set a base location for ourself
    UAB_ASSERT(_playerStartingBaseLocations[BWAPI::Broodwar->self()] != nullptr, "We didn't set a valid selfStartLocation in BaseLocations");
}

void BaseLocationManager::update()
{
    // TODO: update occupied base locations

    drawBaseLocations();
}

void BaseLocationManager::drawBaseLocations() const
{
    for (auto & baseLocation : _baseLocationData)
    {
        baseLocation.draw();
    }
}

std::vector<const BaseLocation *> & BaseLocationManager::getBaseLocations()
{
    return _baseLocationPtrs;
}

std::vector<const BaseLocation *> & BaseLocationManager::getStartingBaseLocations()
{
    return _startingBaseLocations;
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
