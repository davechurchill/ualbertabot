#pragma once

#include "Common.h"
#include <vector>
#include "BWAPI.h"
#include "DistanceMap.hpp"

namespace UAlbertaBot
{
   
// provides useful tools for analyzing the starcraft map
// calculates connectivity and distances using flood fills
class MapTools
{
    // a cache of already computed distance maps
	std::map<BWAPI::Position, DistanceMap>	allMaps;

	// the map stored at TilePosition resolution 
	// values are 0/1 for walkable or not walkable
	std::vector<bool>	map;

	// map that stores whether a unit is on this position
	std::vector<bool>	units;

	// the fringe vector which is used as a sort of 'open list'
	std::vector<int>	fringe;

	// the size of the map
	int					rows,
						cols;

	// constructor for MapTools
	MapTools();
	~MapTools() {}

	// return the index of the 1D array from (row,col)
	inline int getIndex(int row, int col);

	bool unexplored(DistanceMap & dmap, const int index) const;
	
	// resets the distance and fringe vectors, call before each search
	void reset();

	// reads in the map data from bwapi and stores it in our map format
	void setBWAPIMapData();
	
	// reset the fringe
	void resetFringe();

	// computes walk distance from Position P to all other points on the map
	void computeDistance(DistanceMap & dmap, const BWAPI::Position p);

    BWAPI::TilePosition getTilePosition(int index);

	
public:

	static MapTools &	Instance();

	BWAPI::TilePosition getNextExpansion();
	BWAPI::TilePosition getNextExpansion(BWAPI::Player player);

	// does the dynamic programming search
	void search(DistanceMap & dmap, const int sR, const int sC);

	// computes connectivity for the map
	void fill(const int index, const int region);

	// get the ground distance between (from, to)
	int getGroundDistance(BWAPI::Position from, BWAPI::Position to);

    const std::vector<BWAPI::TilePosition> & getClosestTilesTo(BWAPI::Position pos);

	// get distance to various bases
	int	getEnemyBaseDistance(BWAPI::Position p);
	int	getMyBaseDistance(BWAPI::Position p);
	BWAPI::Position getEnemyBaseMoveTo(BWAPI::Position p);

    std::vector<BWAPI::UnitInterface *> getUnitsOfTypeNear(BWAPI::Position & nearTo, int groundDistance, BWAPI::UnitType type);
	
	void parseMap();
};

}