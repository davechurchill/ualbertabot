#pragma once

#include "Common.h"
#include <vector>
#include "BWAPI.h"
#include "DistanceMap.h"
#include "BaseLocation.h"

namespace UAlbertaBot
{

// provides useful tools for analyzing the starcraft map
// calculates connectivity and distances using flood fills
class MapTools
{
    std::map<BWAPI::Position, DistanceMap>   _allMaps;   // a cache of already computed distance maps
    std::vector<bool>       _map;       // the map stored at TilePosition resolution, values are 0/1 for walkable or not walkable
    std::vector<bool>       _units;     // map that stores whether a unit is on this position
    std::vector<int>        _fringe;    // the fringe vector which is used as a sort of 'open list'
    std::vector<int>        _lastSeen;  // the last time any of our units has seen this position on the map
    std::vector<bool>       _buildable;
    std::vector<bool>       _depotBuildable;
    std::vector<bool>       _resourceWithinThree;
    int                     _rows;
    int                     _cols;

    int                     getIndex(int row, int col);   // return the index of the 1D array from (row,col)
    bool                    unexplored(DistanceMap & dmap,const int index) const;
    void                    reset();                           // resets the distance and fringe vectors, call before each search    
    void                    setBWAPIMapData();                 // reads in the map data from bwapi and stores it in our map format
    void                    resetFringe();
    
    BWAPI::TilePosition     getTilePosition(int index);
    void                    search(DistanceMap & dmap,const int sR,const int sC);

public:

    MapTools();

    void                    onStart();
    void                    update();
    void                    parseMap();
    
    int                     getGroundDistance(BWAPI::Position from, BWAPI::Position to);
    bool                    isConnected(BWAPI::Position from, BWAPI::Position to);
    void                    drawLastSeen();
    BWAPI::Position         getLeastRecentlySeenPosition();
    bool                    isBuildable(BWAPI::TilePosition tile, BWAPI::UnitType type);
    bool                    isBuildableTile(BWAPI::TilePosition tile);
    bool                    isDepotBuildableTile(BWAPI::TilePosition tile);
    void                    computeDistance(DistanceMap & dmap, const BWAPI::Position p); // computes walk distance from Position P to all other points on the map

    // static helper functions
    static void             GetUnitsInRadius(std::vector<BWAPI::Unit> & units, BWAPI::Position center, int radius, bool ourUnits, bool oppUnits);

    // returns a list of all tiles on the map, sorted by 4-direcitonal walk distance from the given position
    const std::vector<BWAPI::TilePosition> & getClosestTilesTo(BWAPI::Position pos);
};

}