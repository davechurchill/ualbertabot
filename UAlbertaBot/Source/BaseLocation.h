#pragma once

#include "DistanceMap.h"
#include <map>
#include <vector>

namespace UAlbertaBot
{

class BaseLocation
{
    int		m_baseID            = 0;
    int     m_left              = std::numeric_limits<int>::max();
    int     m_right             = std::numeric_limits<int>::lowest();
    int     m_top               = std::numeric_limits<int>::lowest();
    int     m_bottom            = std::numeric_limits<int>::max();
    bool    m_isStartLocation   = false;
    BWAPI::TilePosition         m_startPosition;

    DistanceMap                 m_distanceMap;

    BWAPI::TilePosition			m_depotPosition;
    BWAPI::Position				m_centerOfResources;
    std::vector<BWAPI::Unit>	m_geysers;
    std::vector<BWAPI::Unit>	m_minerals;

    std::vector<BWAPI::Position> m_mineralPositions;
    std::vector<BWAPI::Position> m_geyserPositions;

    std::map<BWAPI::Player, bool> m_isPlayerOccupying;
    std::map<BWAPI::Player, bool> m_isPlayerStartLocation;
    
public:

    BaseLocation(int baseID, const std::vector<BWAPI::Unit>& resources);

    bool isConnected(const BWAPI::Position& pos) const;
    int getGroundDistance(const BWAPI::Position& pos) const;
    int getGroundDistance(const BWAPI::TilePosition& pos) const;
    bool isStartLocation() const;
    bool isPlayerStartLocation(BWAPI::Player player) const;
    bool isMineralOnly() const;
    bool containsPosition(const BWAPI::Position& pos) const;
    const BWAPI::TilePosition& getDepotPosition() const;
    const BWAPI::Position& getPosition() const;
    const std::vector<BWAPI::Unit>& getGeysers() const;
    const std::vector<BWAPI::Unit>& getMinerals() const;
    bool isOccupiedByPlayer(BWAPI::Player player) const;
    bool isExplored() const;
    bool isInResourceBox(int x, int y) const;

    void setPlayerOccupying(BWAPI::Player player, bool occupying);

    const std::vector<BWAPI::TilePosition>& getClosestTiles() const;

    void draw();
};
}