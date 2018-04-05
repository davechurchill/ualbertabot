#include "BaseLocation.h"
#include "UnitUtil.h"
#include <BWAPI.h>

using namespace UAlbertaBot;

const int NearBaseLocationTileDistance = 20;

BaseLocation::BaseLocation(std::shared_ptr<AKBot::OpponentView> opponentView, shared_ptr<MapTools> mapTools, int baseID)
    : _opponentView(opponentView)
	, _mapTools(mapTools)
	, _baseID               (baseID)
    , _isStartLocation      (false)
	, _geyserPositions(0)
	, _mineralPositions(0)
	, _geysers(0)
	, _minerals(0)
    , _left                 (std::numeric_limits<int>::max())
    , _right                (std::numeric_limits<int>::min())
    , _top                  (std::numeric_limits<int>::max())
    , _bottom               (std::numeric_limits<int>::min())
{
	auto self = opponentView->self();
    _isPlayerStartLocation[self] = false;
    _isPlayerOccupying[self] = false;
	for (const auto& enemyPlayer : opponentView->enemies())
	{
		_isPlayerStartLocation[enemyPlayer] = false;
		_isPlayerOccupying[enemyPlayer] = false;
	}
}

BaseLocation::BaseLocation(std::shared_ptr<AKBot::OpponentView> opponentView, shared_ptr<MapTools> mapTools, int baseID, const std::vector<BWAPI::Unit> & resources)
    : BaseLocation(opponentView, mapTools, baseID)
{
	auto self = opponentView->self();

    // add each of the resources to its corresponding container
    for (auto & resource : resources)
    {
		auto resourcePosition = resource->getPosition();
		auto resourceType = resource->getType();
		if (resourceType.isMineralField())
        {
            _minerals.push_back(resource);
            _mineralPositions.push_back(resourcePosition);
        }
        else
        {
            _geysers.push_back(resource);
            _geyserPositions.push_back(resourcePosition);
        }

        // set the limits of the base location bounding box
		_left   = std::min(_left,   resourcePosition.x - resourceType.dimensionLeft());
        _right  = std::max(_right,  resourcePosition.x + resourceType.dimensionRight());
        _top    = std::min(_top,    resourcePosition.y - resourceType.dimensionUp());
        _bottom = std::max(_bottom, resourcePosition.y + resourceType.dimensionDown());
    }

    // calculate the center of the resources
	_centerOfResources = BWAPI::Position(_left + (_right - _left) / 2, _top + (_bottom - _top) / 2);
    UAB_ASSERT(_centerOfResources.isValid(), "Found an invalid resource center");

    // compute this BaseLocation's DistanceMap, which will compute the ground distance
    // from the center of its recourses to every other tile on the map
    _distanceMap = _mapTools->getDistanceMap(_centerOfResources);

    // check to see if this is a start location for the map
    for (auto & tilePos : BWAPI::Broodwar->getStartLocations())
    {
        BWAPI::Position pos(tilePos);

        if (getGroundTileDistance(pos) < NearBaseLocationTileDistance)
        {
            _isStartLocation = true;
            _position = pos;
            _depotTile = tilePos;
        }
    }

    // if it's our starting location, set the pointer
    if (getDepotTilePosition() == self->getStartLocation())
    {
        _isPlayerStartLocation[self] = true;
    }

    // if it's not a start location, we need to calculate the depot position
    if (!isStartLocation())
    {
        // the position of the depot will be the closest spot we can build one from the resource center
        for (auto & tile : getClosestTiles())
        {
            // the build position will be up-left of where this tile is
            // this means we are positioning the center of the resouce depot
            BWAPI::TilePosition buildTile(tile.x - 1, tile.y - 1);
			auto resourceDepot = UnitUtil::getResourceDepot(self->getRace());
            if (_mapTools->isBuildable(buildTile, resourceDepot))
            {
                _depotTile = buildTile;
                _position = BWAPI::Position(buildTile);
                break;
            }
        }
    }
}

const std::vector<BWAPI::TilePosition> & BaseLocation::getClosestTiles() const
{
    return _distanceMap.getSortedTiles();
}

void BaseLocation::setPlayerOccupying(BWAPI::Player player, bool occupying)
{
    _isPlayerOccupying[player] = occupying;

    // if this base is a start location that's occupied by the enemy, it's that enemy's start location
	bool isEnemyPlayer = player == _opponentView->defaultEnemy();
    if (occupying && isEnemyPlayer && isStartLocation() && _isPlayerStartLocation[player] == false)
    {
        _isPlayerStartLocation[player] = true;
    }
}

bool BaseLocation::isOccupiedByPlayer(BWAPI::Player player) const
{
    return _isPlayerOccupying.at(player);
}

bool BaseLocation::isExplored() const
{
    return _mapTools->isExplored(getDepotTilePosition());
}

bool BaseLocation::isPlayerStartLocation(BWAPI::Player player) const
{
    return _isPlayerStartLocation.at(player);
}

bool BaseLocation::containsPosition(const BWAPI::Position & pos) const
{
    if (!pos.isValid() || (pos.x == 0 && pos.y == 0))
    {
        return false;
    }

    return getGroundTileDistance(pos) < NearBaseLocationTileDistance;
}

const std::vector<BWAPI::Unit> & BaseLocation::getGeysers() const
{
    return _geysers;
}

const std::vector<BWAPI::Unit> & BaseLocation::getMinerals() const
{
    return _minerals;
}

const BWAPI::Position & BaseLocation::getPosition() const
{
    return _position;
}

int BaseLocation::getGroundTileDistance(const BWAPI::Position & pos) const
{
    return _distanceMap.getDistance(pos);
}

bool BaseLocation::isStartLocation() const
{
    return _isStartLocation;
}

const BWAPI::TilePosition & BaseLocation::getDepotTilePosition() const
{
    return _depotTile;
}

bool BaseLocation::isMineralOnly() const
{
    return getGeysers().empty();
}

const std::vector<BWAPI::Position>& BaseLocation::getMineralPositions() const
{
	return _mineralPositions;
}

const std::vector<BWAPI::Position>& BaseLocation::getGeyserPositions() const
{
	return _geyserPositions;
}