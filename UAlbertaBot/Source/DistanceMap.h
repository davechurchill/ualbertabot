#pragma once

#include "Common.h"
#include "Grid.hpp"

namespace UAlbertaBot
{
class DistanceMap
{
    int                             m_width = 0;
    int                             m_height = 0;
    Grid<int>                       m_dist;
    BWAPI::TilePosition             m_startTile;
    std::vector<BWAPI::TilePosition> m_sortedTiles;

public:

    DistanceMap();
    void computeDistanceMap(const BWAPI::TilePosition & startTile);

    int getDistance(int tileX, int tileY) const;
    int getDistance(const BWAPI::TilePosition & pos) const;
    int getDistance(const BWAPI::Position & pos) const;

    // given a position, get the position we should move to to minimize distance
    const std::vector<BWAPI::TilePosition> & getSortedTiles() const;
    const BWAPI::TilePosition & getStartTile() const;

    void draw() const;
};

}