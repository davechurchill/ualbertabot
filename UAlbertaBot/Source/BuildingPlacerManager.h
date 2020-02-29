#pragma once

#include "Common.h"

namespace UAlbertaBot
{

class Building;
class BuildingPlacerManager
{
    friend class Global;

    BuildingPlacerManager();

    std::vector< std::vector<bool> > _reserveMap;

    int _boxTop     = std::numeric_limits<int>::max();
    int _boxBottom  = std::numeric_limits<int>::lowest();
    int _boxLeft    = std::numeric_limits<int>::max();
    int _boxRight   = std::numeric_limits<int>::lowest();

public:

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