#pragma once

#include <vector>
#include "DistanceMap.h"
#include "Grid.hpp"

#include "stardraft/StarDraft.h"

namespace UAlbertaBot
{

class MapTools
{
    friend class Global;
    
    StarDraftMap m_map;

    int     m_width = 0;
    int     m_height = 0;
    int     m_frame = 0;
    
    // a cache of already computed distance maps, which is mutable since it only acts as a cache
    mutable std::map<std::pair<int,int>, DistanceMap> m_allMaps;   

    Grid<int> m_walkable;       // whether a tile is buildable (includes static resources)          
    Grid<int> m_buildable;      // whether a tile is buildable (includes static resources)
    Grid<int> m_depotBuildable; // whether a depot is buildable on a tile (illegal within 3 tiles of static resource)
    Grid<int> m_lastSeen;       // the last time any of our units has seen this position on the map
    Grid<int> m_sectorNumber;   // connectivity sector number, two tiles are ground connected if they have the same number
    
    void computeMap();

    void computeConnectivity();
    int  getSectorNumber(int x, int y) const;
    void printMap() const;
    bool canBuild(int tileX, int tileY) const;
    bool canWalk(int tileX, int tileY) const;

    MapTools();

public:
    
    void    onStart();
    void    onFrame();
    void    draw() const;

    int     width() const;
    int     height() const;

    bool    isValidTile(int tileX, int tileY) const;
    bool    isValidTile(const BWAPI::TilePosition & tile) const;
    bool    isValidPosition(const BWAPI::Position & pos) const;
    bool    isPowered(int tileX, int tileY) const;
    bool    isExplored(int tileX, int tileY) const;
    bool    isExplored(const BWAPI::Position & pos) const;
    bool    isExplored(const BWAPI::TilePosition & pos) const;
    bool    isVisible(int tileX, int tileY) const;
    bool    canBuildTypeAtPosition(int tileX, int tileY, const BWAPI::UnitType & type) const;
    void    drawTile(int tileX, int tileY, const BWAPI::Color & color) const;

    const   DistanceMap & getDistanceMap(const BWAPI::TilePosition & tile) const;
    const   DistanceMap & getDistanceMap(const BWAPI::Position & tile) const;
    int     getGroundDistance(const BWAPI::Position & src, const BWAPI::Position & dest) const;
    bool    isConnected(int x1, int y1, int x2, int y2) const;
    bool    isConnected(const BWAPI::TilePosition & from, const BWAPI::TilePosition & to) const;
    bool    isConnected(const BWAPI::Position & from, const BWAPI::Position & to) const;
    bool    isWalkable(int tileX, int tileY) const;
    bool    isWalkable(const BWAPI::TilePosition & tile) const;
    
    bool    isBuildable(int tileX, int tileY) const;
    bool    isBuildable(const BWAPI::TilePosition & tile) const;
    bool    isDepotBuildableTile(int tileX, int tileY) const;
    
    void    getUnits(BWAPI::Unitset & units, BWAPI::Position center, int radius, bool ourUnits, bool oppUnits);

    const StarDraftMap & getStarDraftMap() const;
    void saveMapToFile(const std::string & path) const;

    BWAPI::TilePosition getLeastRecentlySeenTile() const;

    // returns a list of all tiles on the map, sorted by 4-direcitonal walk distance from the given position
    const std::vector<BWAPI::TilePosition> & getClosestTilesTo(const BWAPI::TilePosition & tilePos) const;
    const std::vector<BWAPI::TilePosition> & getClosestTilesTo(const BWAPI::Position & pos) const;
};

}