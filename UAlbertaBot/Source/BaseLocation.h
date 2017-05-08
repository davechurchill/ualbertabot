#pragma once

#include "Common.h"
#include "MetaType.h"
#include "DistanceMap.h"
#include "UnitData.h"

namespace UAlbertaBot
{

class BaseLocation
{
    BWAPI::Position     _position;
    BWAPI::TilePosition _tile;
    BWAPI::Position     _resourceCenter;
    std::vector<BWAPI::Unit>      _geysers;
    std::vector<BWAPI::Unit>      _minerals;

    std::vector<BWAPI::Position> _mineralPositions;
    std::vector<BWAPI::Position> _geyserPositions;

    std::set<BWAPI::Player> _playersOccupying;
        
    DistanceMap         _distanceMap;
    int                 _baseID;
    int                 _left;
    int                 _right;
    int                 _top;
    int                 _bottom;
    bool                _isStartLocation;
    bool                _isSelfStartLocation;
    bool                _isEnemyStartLocation;
    
public:

    BaseLocation(int baseID = 0);
    BaseLocation(int baseID, const std::vector<BWAPI::Unit> & resources);

    int getGroundTileDistance(const BWAPI::Position & pos) const;
    bool isStartLocation() const;
    bool isSelfStartLocation() const;
    bool isMineralOnly() const;
    bool containsPosition(const BWAPI::Position & pos) const;
    const BWAPI::TilePosition & getTilePosition() const;
    const BWAPI::Position & getPosition() const;
    const std::vector<BWAPI::Unit> & getGeysers() const;
    const std::vector<BWAPI::Unit> & getMinerals() const;

    void update();

    void draw() const;
};


}