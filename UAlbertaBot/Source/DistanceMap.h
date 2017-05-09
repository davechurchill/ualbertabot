#pragma once

#include <vector>
#include "BWAPI.h"

namespace UAlbertaBot
{
class DistanceMap
{
    int _rows;
    int _cols;
    int _startRow;
    int _startCol;

    std::vector<int>	_dist;
    std::vector<char>	_moveTo;
    std::vector<BWAPI::TilePosition>    _sorted;

    int getIndex(const int row,const int col) const;
    int getIndex(const BWAPI::Position & p) const;

public:

    DistanceMap();

    const int & operator [] (const int index) const;
          int & operator [] (const int index);
    const int & operator [] (const BWAPI::Position & pos) const;
    void setMoveTo(const int index,const char val);
    void setDistance(const int index,const int val);
    void setStartPosition(const int sr,const int sc);
    bool isConnected(const BWAPI::Position p) const;
    void addSorted(const BWAPI::TilePosition & tp);

    // given a position, get the position we should move to to minimize distance
    BWAPI::Position getMoveTo(const BWAPI::Position p, const int lookAhead = 1) const;
    const std::vector<BWAPI::TilePosition> & getSortedTiles() const;
};
}