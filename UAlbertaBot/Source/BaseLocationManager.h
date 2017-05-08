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

    
public:

    BaseLocationManager();
    
    void update();
    void onStart();
    void drawBaseLocations() const;

    std::vector<const BaseLocation *> & getBaseLocations();
    std::vector<const BaseLocation *> & getStartingBaseLocations();
    const std::vector<const BaseLocation *> & getBaseLocations() const;
    const std::vector<const BaseLocation *> & getStartingBaseLocations() const;
    const BaseLocation * getPlayerStartingBaseLocation(BWAPI::Player player);
    BWAPI::TilePosition getNextExpansion(BWAPI::Player player);
    const std::set<const BaseLocation *> & getOccupiedBaseLocations(BWAPI::Player player);
};

}