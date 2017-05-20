#include "BaseLocation.h"
#include "Global.h"
#include "UnitUtil.h"

using namespace UAlbertaBot;

const int NearBaseLocationTileDistance = 20;

BaseLocation::BaseLocation(int baseID)
    : _baseID               (baseID)
    , _isStartLocation      (false)
    , _left                 (std::numeric_limits<int>::max())
    , _right                (std::numeric_limits<int>::min())
    , _top                  (std::numeric_limits<int>::max())
    , _bottom               (std::numeric_limits<int>::min())
{
    _isPlayerStartLocation[BWAPI::Broodwar->self()] = false;
    _isPlayerStartLocation[BWAPI::Broodwar->enemy()] = false;
    _isPlayerOccupying[BWAPI::Broodwar->self()] = false;
    _isPlayerOccupying[BWAPI::Broodwar->enemy()] = false;
}

BaseLocation::BaseLocation(int baseID, const std::vector<BWAPI::Unit> & resources)
    : BaseLocation(baseID)
{
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
    _resourceCenter = BWAPI::Position(resourceCenterX / numResources, resourceCenterY / numResources);

    _resourceCenter = BWAPI::Position(_left + (_right-_left)/2, _top + (_bottom-_top)/2);

    UAB_ASSERT(_resourceCenter.isValid(), "Found an invalid resource center");

    // compute this BaseLocation's DistanceMap, which will compute the ground distance
    // from the center of its recourses to every other tile on the map
    _distanceMap = Global::Map().getDistanceMap(_resourceCenter);

    // check to see if this is a start location for the map
    for (auto & tilePos : BWAPI::Broodwar->getStartLocations())
    {
        BWAPI::Position pos(tilePos);

        if (getGroundTileDistance(pos) < NearBaseLocationTileDistance)
        {
            _isStartLocation = true;
            _position = pos;
            _tile = tilePos;
        }
    }

    // if it's our starting location, set the pointer
    if (getTilePosition() == BWAPI::Broodwar->self()->getStartLocation())
    {
        _isPlayerStartLocation[BWAPI::Broodwar->self()] = true;
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

            if (Global::Map().isBuildable(buildTile, BWAPI::Broodwar->self()->getRace().getResourceDepot()))
            {
                _tile = buildTile;
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
    if (occupying && player == BWAPI::Broodwar->enemy() && isStartLocation() && _isPlayerStartLocation[player] == false)
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
    return BWAPI::Broodwar->isExplored(getTilePosition());
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

void BaseLocation::draw()
{
    BWAPI::Broodwar->drawCircleMap(_resourceCenter, 16, BWAPI::Colors::Yellow);

    std::stringstream ss;
    ss << "BaseLocation: " << _baseID << "\n";
    ss << "Start Loc:    " << (isStartLocation() ? "true" : "false") << "\n";
    ss << "Minerals:     " << _mineralPositions.size() << "\n";
    ss << "Geysers:      " << _geyserPositions.size() << "\n";
    ss << "Occupied By:  ";

    if (isOccupiedByPlayer(BWAPI::Broodwar->self()))
    {
        ss << "Self ";
    }

    if (isOccupiedByPlayer(BWAPI::Broodwar->enemy()))
    {
        ss << "Enemy ";
    }

    BWAPI::Broodwar->drawTextMap(_left, _top-60, ss.str().c_str());
    BWAPI::Broodwar->drawTextMap(_left, _bottom, ss.str().c_str());

    // draw the base bounding box
    BWAPI::Broodwar->drawBoxMap(_left, _top, _right, _bottom, BWAPI::Colors::White, false);
    BWAPI::Position mDiag(BWAPI::UnitTypes::Resource_Mineral_Field.dimensionLeft(),
                          BWAPI::UnitTypes::Resource_Mineral_Field.dimensionDown());
    BWAPI::Position gDiag(BWAPI::UnitTypes::Resource_Vespene_Geyser.dimensionLeft(),
                          BWAPI::UnitTypes::Resource_Vespene_Geyser.dimensionDown());
    for (auto & mineralPos : _mineralPositions)
    {
        BWAPI::Broodwar->drawBoxMap(mineralPos - mDiag, mineralPos + mDiag, BWAPI::Colors::Cyan, false);
        //BWAPI::Broodwar->drawLineMap(mineralPos, _resourceCenter, BWAPI::Colors::Cyan);
    }

    for (auto & geyserPos : _geyserPositions)
    {
        BWAPI::Broodwar->drawBoxMap(geyserPos - gDiag, geyserPos + gDiag, BWAPI::Colors::Green, false);
        //BWAPI::Broodwar->drawLineMap(geyserPos, _resourceCenter, BWAPI::Colors::Green);
    }

    if (_isStartLocation)
    {
        BWAPI::Broodwar->drawCircleMap(_position, 10, BWAPI::Colors::Red, true);
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

        BWAPI::Color color = Global::Map().isBuildableTile(tile) ? BWAPI::Colors::Green : BWAPI::Colors::Red;
        if (Global::Map().isBuildableTile(tile) && !Global::Map().isDepotBuildableTile(tile))
        {
            color = BWAPI::Colors::Blue;
        }
        if (Global::Map().isBuildable(tile, BWAPI::UnitTypes::Terran_Command_Center))
        {
            color = BWAPI::Colors::Purple;
        }

        BWAPI::Broodwar->drawCircleMap(pos, 5, color, false);
        BWAPI::Broodwar->drawTextMap(pos, "%d", getGroundTileDistance(pos));
    }

    int ccWidth = BWAPI::UnitTypes::Terran_Command_Center.tileWidth() * 32;
    int ccHeight = BWAPI::UnitTypes::Terran_Command_Center.tileHeight() * 32;
    BWAPI::Broodwar->drawBoxMap(_position, _position + BWAPI::Position(ccWidth, ccHeight), BWAPI::Colors::Red, false);
}

const BWAPI::TilePosition & BaseLocation::getTilePosition() const
{
    return _tile;
}

bool BaseLocation::isMineralOnly() const
{
    return getGeysers().empty();
}