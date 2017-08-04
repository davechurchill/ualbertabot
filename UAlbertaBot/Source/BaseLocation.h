#pragma once

#include "Common.h"
#include "Rect.h"
#include "MetaType.h"
#include "DistanceMap.h"
#include "UnitData.h"
#include "OpponentView.h"
#include "MapTools.h"

namespace UAlbertaBot
{

class BaseLocation
{
    BWAPI::Position                 _position;
    BWAPI::TilePosition             _depotTile;
    BWAPI::Position                 _centerOfResources;
    std::vector<BWAPI::Unit>        _geysers;
    std::vector<BWAPI::Unit>        _minerals;

    std::vector<BWAPI::Position>    _mineralPositions;
    std::vector<BWAPI::Position>    _geyserPositions;

    std::map<BWAPI::Player, bool>   _isPlayerOccupying;
    std::map<BWAPI::Player, bool>   _isPlayerStartLocation;
        
    DistanceMap         _distanceMap;
    int                 _baseID;
    int                 _left;
    int                 _right;
    int                 _top;
    int                 _bottom;
    bool                _isStartLocation;
	const AKBot::OpponentView& _opponentView;
	const MapTools&		_mapTools;
public:

    BaseLocation(const AKBot::OpponentView& opponentView, const MapTools& mapTools, int baseID);
	BaseLocation(const AKBot::OpponentView& opponentView, const MapTools& mapTools, int baseID, const std::vector<BWAPI::Unit> & resources);
    
	/* Get id of the base location */
	int getId() const { return _baseID; };
	AKBot::Rect getRecourceBounds() const { return AKBot::Rect(_left, _top, _right, _bottom); };
    int getGroundTileDistance(const BWAPI::Position & pos) const;
    bool isStartLocation() const;
    bool isPlayerStartLocation(BWAPI::Player player) const;
    bool isMineralOnly() const;
    bool containsPosition(const BWAPI::Position & pos) const;
    const BWAPI::TilePosition & getDepotTilePosition() const;
    const BWAPI::Position & getPosition() const;
    const std::vector<BWAPI::Unit> & getGeysers() const;
    const std::vector<BWAPI::Unit> & getMinerals() const;
    bool isOccupiedByPlayer(BWAPI::Player player) const;
    bool isExplored() const;
	const std::vector<BWAPI::Position>& getMineralPositions() const;
	const std::vector<BWAPI::Position>& getGeyserPositions() const;

    void setPlayerOccupying(BWAPI::Player player, bool occupying);

    const std::vector<BWAPI::TilePosition> & getClosestTiles() const;
};


}