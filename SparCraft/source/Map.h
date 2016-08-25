#pragma once

#include "Common.h"
#include "Array.hpp"
#include "Unit.h"

#include <iostream>
#include <fstream>

namespace SparCraft
{

typedef std::vector< std::vector<bool> > bvv;

class Map
{
	size_t					_walkTileWidth;
	size_t					_walkTileHeight;
	size_t					_buildTileWidth;
	size_t					_buildTileHeight;
	bvv						_mapData;	            // true if walk tile [x][y] is walkable

	bvv						_unitData;	            // true if unit on build tile [x][y]
	bvv						_buildingData;          // true if building on build tile [x][y]

    const Position getWalkPosition(const Position & pixelPosition) const;

    void resetVectors();

public:

    Map();

    // constructor which sets a completely walkable map
    Map(const size_t & bottomRightBuildTileX, const size_t & bottomRightBuildTileY);

    Map(BWAPI::GameWrapper & game);
	
    const size_t getPixelWidth() const;
    const size_t getPixelHeight() const;
    const size_t & getWalkTileWidth() const;
    const size_t & getWalkTileHeight() const;
    const size_t & getBuildTileWidth() const;
    const size_t & getBuildTileHeight() const;

    const bool isWalkable(const SparCraft::Position & pixelPosition) const;
    const bool isFlyable(const SparCraft::Position & pixelPosition) const;
    const bool isWalkable(const int & walkTileX, const int & walkTileY) const;
    const bool isFlyable(const int & walkTileX, const int & walkTileY) const;
    const bool getMapData(const int & walkTileX, const int & walkTileY) const;
    const bool getUnitData(const int & buildTileX, const int & buildTileY) const;
    const bool canBuildHere(BWAPI::TilePosition pos) const;

    void setMapData(const size_t & walkTileX, const size_t & walkTileY, const bool val);
    void setUnitData(BWAPI::GameWrapper & game);
    void setBuildingData(BWAPI::GameWrapper & game);
    void addUnit(BWAPI::Unit & unit);

    unsigned int * getRGBATexture();

    void write(const std::string & filename);
    void load(const std::string & filename);
};
}