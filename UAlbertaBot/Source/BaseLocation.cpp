#include "BaseLocation.h"
#include "Global.h"
#include "UnitUtil.h"

using namespace UAlbertaBot;

const int NearBaseLocationTileDistance = 15;

BaseLocation::BaseLocation(int baseID)
    : _baseID               (baseID)
    , _isStartLocation      (false)
    , _isSelfStartLocation  (false)
    , _isEnemyStartLocation (false)
    , _left                 (std::numeric_limits<int>::max())
    , _right                (std::numeric_limits<int>::min())
    , _top                  (std::numeric_limits<int>::max())
    , _bottom               (std::numeric_limits<int>::min())
{

}

BaseLocation::BaseLocation(int baseID, const std::vector<BWAPI::Unit> & resources)
    : BaseLocation(baseID)
{
    // add each of the resources to its corresponding container
    for (auto & resource : resources)
    {
        if (resource->getType().isMineralField())
        {
            _minerals.push_back(resource);
            _mineralPositions.push_back(resource->getPosition());
        }
        else
        {
            _geysers.push_back(resource);
            _geyserPositions.push_back(resource->getPosition());
        }

        // set the limits of the base location bounding box
        _left   = std::min(_left,   resource->getPosition().x - resource->getType().dimensionLeft());
        _right  = std::max(_right,  resource->getPosition().x + resource->getType().dimensionRight());
        _top    = std::min(_top,    resource->getPosition().y - resource->getType().dimensionUp());
        _bottom = std::max(_bottom, resource->getPosition().y + resource->getType().dimensionDown());
    }

    // calculate the center of the resources
    _resourceCenter = UnitUtil::GetUnitsetCenter(resources);

    // compute this BaseLocation's DistanceMap, which will compute the ground distance
    // from the center of its recourses to every other tile on the map
    Global::Map().computeDistance(_distanceMap, _resourceCenter);

    // check to see if this is a start location for the map
    for (auto & tilePos : BWAPI::Broodwar->getStartLocations())
    {
        BWAPI::Position pos(tilePos);

        if (getGroundTileDistance(pos) < 12)
        {
            _isStartLocation = true;
            _position = pos;
            _tile = tilePos;
        }
    }

    // if it's our starting location, set the pointer
    if (getTilePosition() == BWAPI::Broodwar->self()->getStartLocation())
    {
        _isSelfStartLocation = true;
    }
}

bool BaseLocation::containsPosition(const BWAPI::Position & pos) const
{
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

bool BaseLocation::isSelfStartLocation() const
{
    return _isSelfStartLocation;
}

int BaseLocation::getGroundTileDistance(const BWAPI::Position & pos) const
{
    return _distanceMap[pos];
}

bool BaseLocation::isStartLocation() const
{
    return _isStartLocation;
}

void BaseLocation::draw() const
{
    BWAPI::Broodwar->drawCircleMap(_resourceCenter, 300, BWAPI::Colors::Yellow);

    std::stringstream ss;
    ss << "BaseLocation: " << _baseID << "\n";
    ss << "Start Loc:    " << (isStartLocation() ? "true" : "false") << "\n";
    ss << "Minerals:     " << _mineralPositions.size() << "\n";
    ss << "Geysers:      "  << _geyserPositions.size() << "\n";
    BWAPI::Broodwar->drawTextMap(_resourceCenter, ss.str().c_str());

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
}

const BWAPI::TilePosition & BaseLocation::getTilePosition() const
{
    return _tile;
}

bool BaseLocation::isMineralOnly() const
{
    return getGeysers().empty();
}