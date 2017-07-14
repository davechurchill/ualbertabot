#include "BaseLocation.h"
#include "Global.h"
#include "UnitUtil.h"

using namespace UAlbertaBot;

const int NearBaseLocationTileDistance = 20;

BaseLocation::BaseLocation(const AKBot::OpponentView& opponentView, int baseID)
    : _opponentView(opponentView)
	, _baseID               (baseID)
    , _isStartLocation      (false)
    , _left                 (std::numeric_limits<int>::max())
    , _right                (std::numeric_limits<int>::min())
    , _top                  (std::numeric_limits<int>::max())
    , _bottom               (std::numeric_limits<int>::min())
{
	auto self = opponentView.self();
    _isPlayerStartLocation[self] = false;
    _isPlayerOccupying[self] = false;
	for (const auto& enemyPlayer : opponentView.enemies())
	{
		_isPlayerStartLocation[enemyPlayer] = false;
		_isPlayerOccupying[enemyPlayer] = false;
	}
}

BaseLocation::BaseLocation(const AKBot::OpponentView& opponentView, int baseID, const std::vector<BWAPI::Unit> & resources)
    : BaseLocation(opponentView, baseID)
{
	auto self = opponentView.self();
	int resourceCenterX = 0;
    int resourceCenterY = 0;

    // add each of the resources to its corresponding container
    for (auto & resource : resources)
    {
        if (resource->getType().isMineralField())
        {
            _minerals.push_back(resource);
            _mineralPositions.push_back(resource->getPosition());

            // add the position of the minerals to the center
            resourceCenterX += resource->getPosition().x;
            resourceCenterY += resource->getPosition().y;
        }
        else
        {
            _geysers.push_back(resource);
            _geyserPositions.push_back(resource->getPosition());

            // pull the resource center toward the geyser if it exists
            resourceCenterX += resource->getPosition().x;
            resourceCenterY += resource->getPosition().y;
        }

        // set the limits of the base location bounding box
        _left   = std::min(_left,   resource->getPosition().x - resource->getType().dimensionLeft());
        _right  = std::max(_right,  resource->getPosition().x + resource->getType().dimensionRight());
        _top    = std::min(_top,    resource->getPosition().y - resource->getType().dimensionUp());
        _bottom = std::max(_bottom, resource->getPosition().y + resource->getType().dimensionDown());
    }

    // calculate the center of the resources
    size_t numResources = _minerals.size() + _geysers.size();
    //_resourceCenter = BWAPI::Position(resourceCenterX / numResources, resourceCenterY / numResources);

    _centerOfResources = BWAPI::Position(_left + (_right-_left)/2, _top + (_bottom-_top)/2);

    UAB_ASSERT(_centerOfResources.isValid(), "Found an invalid resource center");

    // compute this BaseLocation's DistanceMap, which will compute the ground distance
    // from the center of its recourses to every other tile on the map
    _distanceMap = Global::Map().getDistanceMap(_centerOfResources);

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
            if (Global::Map().isBuildable(buildTile, resourceDepot))
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
	bool isEnemyPlayer = player == Global::getEnemy();
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
    return BWAPI::Broodwar->isExplored(getDepotTilePosition());
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

void BaseLocation::draw(AKBot::ScreenCanvas& canvas, std::function<bool(BWAPI::TilePosition tile)> isBuildableTile)
{
    canvas.drawCircleMap(_centerOfResources, 16, BWAPI::Colors::Yellow);

    std::stringstream ss;
    ss << "BaseLocation: " << _baseID << "\n";
    ss << "Start Loc:    " << (isStartLocation() ? "true" : "false") << "\n";
    ss << "Minerals:     " << _mineralPositions.size() << "\n";
    ss << "Geysers:      " << _geyserPositions.size() << "\n";
    ss << "Occupied By:  ";

    if (isOccupiedByPlayer(_opponentView.self()))
    {
        ss << "Self ";
    }

	for (auto& enemy : _opponentView.enemies())
	{
		if (isOccupiedByPlayer(enemy))
		{
			ss << "Enemy " << enemy->getName() << " ";
			break;
		}
	}

    canvas.drawTextMap(_left, _top-60, ss.str().c_str());
    canvas.drawTextMap(_left, _bottom, ss.str().c_str());

    // draw the base bounding box
    canvas.drawBoxMap(_left, _top, _right, _bottom, BWAPI::Colors::White, false);
    BWAPI::Position mDiag(BWAPI::UnitTypes::Resource_Mineral_Field.dimensionLeft(),
                          BWAPI::UnitTypes::Resource_Mineral_Field.dimensionDown());
    BWAPI::Position gDiag(BWAPI::UnitTypes::Resource_Vespene_Geyser.dimensionLeft(),
                          BWAPI::UnitTypes::Resource_Vespene_Geyser.dimensionDown());
    for (auto & mineralPos : _mineralPositions)
    {
        canvas.drawBoxMap(mineralPos - mDiag, mineralPos + mDiag, BWAPI::Colors::Cyan, false);
        //canvas.drawLineMap(mineralPos, _resourceCenter, BWAPI::Colors::Cyan);
    }

    for (auto & geyserPos : _geyserPositions)
    {
        canvas.drawBoxMap(geyserPos - gDiag, geyserPos + gDiag, BWAPI::Colors::Green, false);
        //canvas.drawLineMap(geyserPos, _resourceCenter, BWAPI::Colors::Green);
    }

    if (_isStartLocation)
    {
        canvas.drawCircleMap(_position, 10, BWAPI::Colors::Red, true);
    }

    auto & closestTiles = getClosestTiles();
    if (false) for (size_t i=0; i<200 && i < closestTiles.size(); ++i)
    {
        const BWAPI::TilePosition & tile = closestTiles[i];

        BWAPI::Position pos = BWAPI::Position(tile) + BWAPI::Position(16,16);

        if (!pos.isValid())
        {
            continue;
        }

        BWAPI::Color color = isBuildableTile(tile) ? BWAPI::Colors::Green : BWAPI::Colors::Red;
        if (isBuildableTile(tile) && !Global::Map().isDepotBuildableTile(tile))
        {
            color = BWAPI::Colors::Blue;
        }

		if (Global::Map().isBuildable(tile, BWAPI::UnitTypes::Terran_Command_Center))
        {
            color = BWAPI::Colors::Purple;
        }

        canvas.drawCircleMap(pos, 5, color, false);
        canvas.drawTextMap(pos, "%d", getGroundTileDistance(pos));
    }

    int ccWidth = BWAPI::UnitTypes::Terran_Command_Center.tileWidth() * 32;
    int ccHeight = BWAPI::UnitTypes::Terran_Command_Center.tileHeight() * 32;
    canvas.drawBoxMap(_position, _position + BWAPI::Position(ccWidth, ccHeight), BWAPI::Colors::Red, false);
}

const BWAPI::TilePosition & BaseLocation::getDepotTilePosition() const
{
    return _depotTile;
}

bool BaseLocation::isMineralOnly() const
{
    return getGeysers().empty();
}