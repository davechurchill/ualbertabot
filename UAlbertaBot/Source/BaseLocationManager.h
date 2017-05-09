#pragma once

#include "Common.h"
#include "MetaType.h"
#include "DistanceMap.h"
#include "BaseLocation.h"

namespace UAlbertaBot
{

class BaseLocationManager
{
    std::vector<BaseLocation>   _baseLocationData;

    std::vector<const BaseLocation *> _baseLocationPtrs;
    std::vector<const BaseLocation *> _startingBaseLocations;
    std::map<BWAPI::Player, const BaseLocation *> _playerStartingBaseLocations;
    std::map<BWAPI::Player, std::set<const BaseLocation *>> _occupiedBaseLocations;
    
    std::vector<std::vector<BaseLocation *>> _tileBaseLocations;  // precomputed base locations for each tile on the map, nullptr if not in a base
    
    BaseLocation * _getBaseLocation(BWAPI::TilePosition pos) const;
    BaseLocation * _getBaseLocation(BWAPI::Position pos) const;

public:

    BaseLocationManager();
    
    void update();
    void onStart();
    void drawBaseLocations();

    const std::vector<const BaseLocation *> & getBaseLocations() const;
    const std::vector<const BaseLocation *> & getStartingBaseLocations() const;
    const std::set<const BaseLocation *> & getOccupiedBaseLocations(BWAPI::Player player);
    const BaseLocation * getPlayerStartingBaseLocation(BWAPI::Player player);
    BWAPI::TilePosition getNextExpansion(BWAPI::Player player);

    const BaseLocation * getBaseLocation(BWAPI::TilePosition pos) const;
    const BaseLocation * getBaseLocation(BWAPI::Position pos) const;
};

}