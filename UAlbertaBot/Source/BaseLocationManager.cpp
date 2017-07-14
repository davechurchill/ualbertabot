#include "BaseLocationManager.h"
#include "Global.h"
#include "UnitUtil.h"

using namespace UAlbertaBot;

BaseLocationManager::BaseLocationManager(const AKBot::OpponentView& opponentView)
    : _tileBaseLocations(BWAPI::Broodwar->mapWidth(), std::vector<BaseLocation *>(BWAPI::Broodwar->mapHeight(), nullptr))
	, _opponentView(opponentView)
{
    _playerStartingBaseLocations[_opponentView.self()]  = nullptr;
	for (const auto& enemyPlayer : _opponentView.enemies())
	{
		_playerStartingBaseLocations[enemyPlayer] = nullptr;
	}
}

void BaseLocationManager::onStart(const MapTools& map)
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
            int groundDist = map.getGroundDistance(mineral->getPosition(), UnitUtil::GetUnitsetCenter(cluster));

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
            int groundDist = map.getGroundDistance(geyser->getPosition(), UnitUtil::GetUnitsetCenter(cluster));

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
            _baseLocationData.push_back(BaseLocation(_opponentView, baseID++, cluster));
        }
    }

    // construct the vectors of base location pointers, this is safe since they will never change
	auto self = _opponentView.self();
	auto enemy = Global::getEnemy();
	if (enemy == nullptr)
	{
		return;
	}

	for (auto & baseLocation : _baseLocationData)
    {
        _baseLocationPtrs.push_back(&baseLocation);

        // if it's a start location, add it to the start locations
        if (baseLocation.isStartLocation())
        {
            _startingBaseLocations.push_back(&baseLocation);
        }

        // if it's our starting location, set the pointer
        if (baseLocation.isPlayerStartLocation(self))
        {
            _playerStartingBaseLocations[self] = &baseLocation;
        }

        if (baseLocation.isPlayerStartLocation(enemy))
        {
            _playerStartingBaseLocations[enemy] = &baseLocation;
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
    _occupiedBaseLocations[self] = std::set<const BaseLocation *>();
    _occupiedBaseLocations[enemy] = std::set<const BaseLocation *>();

    // check to see that we have set a base location for ourself
    UAB_ASSERT(_playerStartingBaseLocations[_opponentView.self()] != nullptr, "We didn't set a valid selfStartLocation in BaseLocations");
}

void BaseLocationManager::update(const UnitInfoManager & unitManager)
{   
    // reset the player occupation information for each location
	resetPlayerOccupation();

    // for each unit on the map, update which base location it may be occupying
	auto self = _opponentView.self();
    for (auto & unit : self->getUnits())
    {
        // we only care about buildings on the ground
        if (!unit->getType().isBuilding() || unit->isLifted())
        {
            continue;
        }

        BaseLocation * baseLocation = _getBaseLocation(unit->getPosition());

        if (baseLocation != nullptr)
        {
            baseLocation->setPlayerOccupying(unit->getPlayer(), true);
        }
    }

    auto enemyPlayer = Global::getEnemy();
	if (enemyPlayer == nullptr)
	{
		return;
	}

	// update enemy base occupations
	for (const auto& enemyPlayer : _opponentView.enemies())
	{
		for (const auto & kv : unitManager.getUnitInfoMap(enemyPlayer))
		{
			const UnitInfo & ui = kv.second;

			if (!ui.type.isBuilding())
			{
				continue;
			}

			BaseLocation * baseLocation = _getBaseLocation(ui.lastPosition);

			if (baseLocation != nullptr)
			{
				baseLocation->setPlayerOccupying(enemyPlayer, true);
			}
		}

		// update the starting locations of the enemy player
		// this will happen one of two ways:

		// 1. we've seen the enemy base directly, so the baselocation will know
		if (_playerStartingBaseLocations[enemyPlayer] == nullptr)
		{
			for (auto & baseLocation : _baseLocationData)
			{
				if (baseLocation.isPlayerStartLocation(enemyPlayer))
				{
					_playerStartingBaseLocations[enemyPlayer] = &baseLocation;
				}
			}
		}

		// 2. we've explored every other start location and haven't seen the enemy yet
		if (_playerStartingBaseLocations[enemyPlayer] == nullptr)
		{
			int numStartLocations = getStartingBaseLocations().size();
			int numExploredLocations = 0;
			BaseLocation * unexplored = nullptr;

			for (auto & baseLocation : _baseLocationData)
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
				_playerStartingBaseLocations[enemyPlayer] = unexplored;
				unexplored->setPlayerOccupying(enemyPlayer, true);
			}
		}
	}

    // update the occupied base locations for each player
    _occupiedBaseLocations[self] = std::set<const BaseLocation *>();
	for (const auto& enemyPlayer : _opponentView.enemies())
	{
		_occupiedBaseLocations[enemyPlayer] = std::set<const BaseLocation *>();
	}

    for (auto & baseLocation : _baseLocationData)
    {
        if (baseLocation.isOccupiedByPlayer(self))
        {
            _occupiedBaseLocations[self].insert(&baseLocation);
        }

		for (const auto& enemyPlayer : _opponentView.enemies())
		{
			if (baseLocation.isOccupiedByPlayer(enemyPlayer))
			{
				_occupiedBaseLocations[enemyPlayer].insert(&baseLocation);
			}
		}
    }
}

void UAlbertaBot::BaseLocationManager::resetPlayerOccupation()
{
	auto self = _opponentView.self();
	for (auto & baseLocation : _baseLocationData)
	{
		baseLocation.setPlayerOccupying(self, false);
		for (const auto& enemyPlayer : _opponentView.enemies())
		{
			baseLocation.setPlayerOccupying(enemyPlayer, false);
		}
	}
}

void BaseLocationManager::drawBaseLocations(AKBot::ScreenCanvas& canvas)
{
	auto isBuildableTileCheck = [](BWAPI::TilePosition tile)
	{
		return Global::Map().isBuildableTile(tile);
	};
    for (auto & baseLocation : _baseLocationData)
    {
        baseLocation.draw(canvas, isBuildableTileCheck);
    }

	auto self = _opponentView.self();
	BWAPI::Position nextExpansionPosition(getNextExpansion(self));

    canvas.drawCircleMap(nextExpansionPosition, 16, BWAPI::Colors::Orange, true);
}

const std::vector<const BaseLocation *> & BaseLocationManager::getBaseLocations() const
{
    return _baseLocationPtrs;
}

const std::vector<const BaseLocation *> & BaseLocationManager::getStartingBaseLocations() const
{
    return _startingBaseLocations;
}

const BaseLocation * BaseLocationManager::getPlayerStartingBaseLocation(BWAPI::Player player) const
{
    return _playerStartingBaseLocations.at(player);
}

const std::set<const BaseLocation *> & BaseLocationManager::getOccupiedBaseLocations(BWAPI::Player player) const
{
    return _occupiedBaseLocations.at(player);
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

BWAPI::TilePosition BaseLocationManager::getNextExpansion(BWAPI::Player player) const
{
    const BaseLocation * homeBase = getPlayerStartingBaseLocation(player);
    const BaseLocation * closestBase = nullptr;
    int minDistance = std::numeric_limits<int>::max();

	auto self = _opponentView.self();
	if (!homeBase)
	{
		return BWAPI::TilePositions::None;
	}

	BWAPI::TilePosition homeTile = homeBase->getDepotTilePosition();
    
    // for each base location
    for (auto & base : getBaseLocations())
    {
        // skip mineral only and starting locations (TODO: fix this)
        if (base->isMineralOnly() || base->isStartLocation())
        {
            continue;
        }

        // get the tile position of the base
        BWAPI::TilePosition tile = base->getDepotTilePosition();
        bool buildingInTheWay = false;

		auto resourceDepot = UnitUtil::getResourceDepot(self->getRace());
        for (int x = 0; x < resourceDepot.tileWidth(); ++x)
        {
            for (int y = 0; y < resourceDepot.tileHeight(); ++y)
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
        double distanceFromHome = homeBase->getGroundTileDistance(BWAPI::Position(tile));

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

    if (closestBase)
    {
        return closestBase->getDepotTilePosition();
    }
    else
    {
        return BWAPI::TilePositions::None;
    }
}
