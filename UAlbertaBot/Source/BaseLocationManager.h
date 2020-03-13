#pragma once

#include "BaseLocation.h"
#include "stardraft/BaseBorderFinder.hpp"

namespace UAlbertaBot
{

class BaseLocationManager
{
    friend class Global;

    BaseBorderFinder m_baseBorders;

    std::vector<BaseLocation>                               m_baseLocationData;
    std::vector<const BaseLocation *>                       m_baseLocationPtrs;
    std::vector<const BaseLocation *>                       m_startingBaseLocations;
    std::map<BWAPI::Player, const BaseLocation *>           m_playerStartingBaseLocations;
    std::map<BWAPI::Player, std::set<const BaseLocation *>> m_occupiedBaseLocations;
    std::vector<std::vector<BaseLocation *>>                m_tileBaseLocations;

    BaseLocation * getBaseLocation(const BWAPI::Position & pos) const;

    BaseLocationManager();

public:

    void onStart();
    void onFrame();
    void drawBaseLocations();

    BWAPI::Position calcCenter(const std::vector<BWAPI::Unit> & units);

    const std::vector<const BaseLocation *> & getBaseLocations() const;
    const std::vector<const BaseLocation *> & getStartingBaseLocations() const;
    const std::set<const BaseLocation *> & getOccupiedBaseLocations(BWAPI::Player player) const;
    const BaseLocation * getPlayerStartingBaseLocation(BWAPI::Player player) const;

    BWAPI::TilePosition getNextExpansion(BWAPI::Player player) const;

};
}