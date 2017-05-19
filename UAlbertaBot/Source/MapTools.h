#pragma once

#include "Common.h"
#include <vector>
#include "BWAPI.h"
#include "DistanceMap.h"
#include "BaseLocation.h"

namespace UAlbertaBot
{

class MapTools
{
    // a cache of already computed distance maps, which is mutable since it only acts as a cache
    mutable std::map<BWAPI::Position, DistanceMap>   _allMaps;   

    std::vector<bool>       _map;               // the map stored at TilePosition resolution, values are 0/1 for walkable or not walkable
    std::vector<bool>       _units;             // map that stores whether a unit is on this position
    std::vector<int>        _lastSeen;          // the last time any of our units has seen this position on the map
    std::vector<bool>       _buildable;         // whether a tile is buildable (includes static resources)
    std::vector<bool>       _depotBuildable;    // whether a depot is buildable on a tile (illegal within 3 tiles of static resource)
    int                     _rows;              // TilePosition rows in the map
    int                     _cols;              // TilePosition columns in the map

    int                     getIndex(int row, int col) const;   // return the index of the 1D array from (row,col)
    bool                    unexplored(DistanceMap & dmap,const int index) const;
    void                    setBWAPIMapData();                 // reads in the map data from bwapi and stores it in our map format
    
    BWAPI::TilePosition     getTilePosition(int index) const;
    void                    search(DistanceMap & dmap,const int sR,const int sC) const;

public:

    MapTools();

    void                    onStart();
    void                    update();
    void                    parseMap();
    
    int                     getGroundDistance(BWAPI::Position from, BWAPI::Position to) const;
    bool                    isConnected(BWAPI::Position from, BWAPI::Position to) const;
    void                    drawLastSeen() const;
    BWAPI::Position         getLeastRecentlySeenPosition() const;
    bool                    isBuildable(BWAPI::TilePosition tile, BWAPI::UnitType type) const;
    bool                    isBuildableTile(BWAPI::TilePosition tile) const;
    bool                    isDepotBuildableTile(BWAPI::TilePosition tile) const;
    void                    computeDistance(DistanceMap & dmap, const BWAPI::Position p) const; // computes walk distance from Position P to all other points on the map

    // static helper functions
    static void             GetUnitsInRadius(std::vector<BWAPI::Unit> & units, BWAPI::Position center, int radius, bool ourUnits, bool oppUnits);

    // returns a list of all tiles on the map, sorted by 4-direcitonal walk distance from the given position
    const std::vector<BWAPI::TilePosition> & getClosestTilesTo(BWAPI::Position pos) const;
};

}