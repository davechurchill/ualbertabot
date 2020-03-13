#pragma once

#include "Common.h"
#include "Grid.hpp"

namespace UAlbertaBot
{

class Building;
class BuildingPlacerManager
{
    Grid<int> m_reserveMap;

    int m_boxTop     = std::numeric_limits<int>::max();
    int m_boxBottom  = std::numeric_limits<int>::lowest();
    int m_boxLeft    = std::numeric_limits<int>::max();
    int m_boxRight   = std::numeric_limits<int>::lowest();

public:
    
    BuildingPlacerManager();

    // queries for various BuildingPlacer data
    bool buildable(const Building & b,int x,int y) const;
    bool isReserved(int x,int y) const;
    bool isInResourceBox(int x,int y) const;
    bool tileOverlapsBaseLocation(BWAPI::TilePosition tile,BWAPI::UnitType type) const;
    bool tileBlocksAddon(BWAPI::TilePosition position) const;

    BWAPI::TilePosition GetBuildLocation(const Building & b,int padding) const;

    // determines whether we can build at a given location
    bool canBuildHere(BWAPI::TilePosition position,const Building & b) const;
    bool canBuildHereWithSpace(BWAPI::TilePosition position,const Building & b,int buildDist,bool horizontalOnly = false) const;

    // returns a build location near a building's desired location
    BWAPI::TilePosition getBuildLocationNear(const Building & b,int buildDist,bool horizontalOnly = false) const;

    void reserveTiles(BWAPI::TilePosition position,int width,int height);
    void freeTiles(BWAPI::TilePosition position,int width,int height);

    void drawReservedTiles();
    void computeResourceBox();

    BWAPI::TilePosition getRefineryPosition();
};
}