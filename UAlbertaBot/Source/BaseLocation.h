#pragma once

#include "Common.h"
#include "MetaType.h"
#include "DistanceMap.h"
#include "UnitData.h"
#include "ScreenCanvas.h"

namespace UAlbertaBot
{

class BaseLocation
{
    BWAPI::Position                 _position;
    BWAPI::TilePosition             _depotTile;
    BWAPI::Position                 _centerOfResources;
    std::vector<BWAPI::Unit>        _geysers;
    std::vector<BWAPI::Unit>        _minerals;

    std::vector<BWAPI::Position>    _mineralPositions;
    std::vector<BWAPI::Position>    _geyserPositions;

    std::map<BWAPI::Player, bool>   _isPlayerOccupying;
    std::map<BWAPI::Player, bool>   _isPlayerStartLocation;
        
    DistanceMap         _distanceMap;
    int                 _baseID;
    int                 _left;
    int                 _right;
    int                 _top;
    int                 _bottom;
    bool                _isStartLocation;
    
public:

    BaseLocation(int baseID = 0);
    BaseLocation(int baseID, const std::vector<BWAPI::Unit> & resources);
    
    int getGroundTileDistance(const BWAPI::Position & pos) const;
    bool isStartLocation() const;
    bool isPlayerStartLocation(BWAPI::Player player) const;
    bool isMineralOnly() const;
    bool containsPosition(const BWAPI::Position & pos) const;
    const BWAPI::TilePosition & getDepotTilePosition() const;
    const BWAPI::Position & getPosition() const;
    const std::vector<BWAPI::Unit> & getGeysers() const;
    const std::vector<BWAPI::Unit> & getMinerals() const;
    bool isOccupiedByPlayer(BWAPI::Player player) const;
    bool isExplored() const;

    void setPlayerOccupying(BWAPI::Player player, bool occupying);

    const std::vector<BWAPI::TilePosition> & getClosestTiles() const;

    void draw(AKBot::ScreenCanvas& canvas, std::function<bool(BWAPI::TilePosition tile)> isBuildableTile);
};


}