#pragma once

#include "Common.h"
#include <vector>
#include <BWAPI/Position.h>
#include "DistanceMap.h"
#include "Logger.h"
#include "OpponentView.h"
#include "MapInformation.h"

namespace UAlbertaBot
{
    
class MapTools
{
    size_t _width;
    size_t _height;

    // a cache of already computed distance maps, which is mutable since it only acts as a cache
    mutable std::map<BWAPI::TilePosition, DistanceMap>   _allMaps;
	std::shared_ptr<AKBot::Logger> _logger;
	shared_ptr<AKBot::MapInformation> _mapInformation;

    std::vector<std::vector<bool>> _walkable;               // the map stored at TilePosition resolution, values are 0/1 for walkable or not walkable
    std::vector<std::vector<bool>> _buildable;         // whether a tile is buildable (includes static resources)
    std::vector<std::vector<bool>> _depotBuildable;    // whether a depot is buildable on a tile (illegal within 3 tiles of static resource)
    std::vector<std::vector<int>>  _lastSeen;          // the last time any of our units has seen this position on the map
    std::vector<std::vector<int>>  _sectorNumber;      // connectivity sector number, two tiles are ground connected if they have the same number
    
    void setBWAPIMapData();                 // reads in the map data from bwapi and stores it in our map format
    void computeConnectivity();

    const int & getSectorNumber(const BWAPI::TilePosition & tile) const;
    int & getSectorNumber(const BWAPI::TilePosition & tile);
    
public:

    MapTools(shared_ptr<AKBot::MapInformation> mapInformation, std::shared_ptr<AKBot::Logger> logger);

	const size_t getWidth() const { return _width; };
	const size_t getHeight() const { return _height; };

    void                    onStart();
    void                    update(int currentFrame);
    
    const DistanceMap &     getDistanceMap(const BWAPI::Position & pos) const;
    const DistanceMap &     getDistanceMap(const BWAPI::TilePosition & tile) const;
    int                     getGroundDistance(const BWAPI::TilePosition & src, const BWAPI::TilePosition & dest) const;
    int                     getGroundDistance(const BWAPI::Position & src, const BWAPI::Position & dest) const;
    bool                    isConnected(const BWAPI::TilePosition & from, const BWAPI::TilePosition & to) const;
    bool                    isConnected(const BWAPI::Position & from, const BWAPI::Position & to) const;
    bool                    isWalkable(const BWAPI::TilePosition & tile) const;
    bool                    isWalkable(const BWAPI::Position & pos) const;
    bool                    isBuildable(BWAPI::TilePosition tile, BWAPI::UnitType type) const;
    bool                    isBuildableTile(BWAPI::TilePosition tile) const;
    bool                    isDepotBuildableTile(BWAPI::TilePosition tile) const;
    

    // returns a list of all tiles on the map, sorted by 4-direcitonal walk distance from the given position
    const std::vector<BWAPI::TilePosition> & getClosestTilesTo(const BWAPI::TilePosition & tile) const;
    const std::vector<BWAPI::TilePosition> & getClosestTilesTo(BWAPI::Position pos) const;
	int getLastSeen(int x, int y) const;
};

}