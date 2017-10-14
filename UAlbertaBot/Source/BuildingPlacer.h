#pragma once

#include "Common.h"
#include "BuildingData.h"
#include "MetaType.h"
#include "BaseLocationManager.h"
#include "OpponentView.h"

namespace UAlbertaBot
{
	enum BuildingPlaceCheckStatus
	{
		CanBuild,
		CannotBuild,
		LocationReserved,
		BaseLocationOverlap,
		ResourceOverlap
	};

class BuildingPlacer
{
    std::vector< std::vector<bool> > _reserveMap;
	int _width;
	int _height;
    int     _boxTop;
    int	    _boxBottom;
    int	    _boxLeft;
    int	    _boxRight;
	shared_ptr<BaseLocationManager> _bases;
	shared_ptr<AKBot::OpponentView> _opponentView;
	shared_ptr<MapTools> _mapTools;

    void    computeBuildableTileDistance(BWAPI::TilePosition tp);

public:
    
    BuildingPlacer(
		int width,
		int height,
		shared_ptr<AKBot::OpponentView> opponentView,
		shared_ptr<BaseLocationManager> bases,
		shared_ptr<MapTools> mapTools);

    // queries for various BuildingPlacer data
    bool					buildable(const Building & b,int x,int y) const;
    bool					isReserved(int x,int y) const;
    bool					isInResourceBox(int x,int y) const;
    bool					tileOverlapsBaseLocation(BWAPI::TilePosition tile,BWAPI::UnitType type) const;
    bool                    tileBlocksAddon(BWAPI::TilePosition position) const;

    // determines whether we can build at a given location
	BuildingPlaceCheckStatus	canBuildHere(BWAPI::TilePosition position,const Building & b) const;
	BuildingPlaceCheckStatus	canBuildHereWithSpace(BWAPI::TilePosition position,const Building & b,int buildDist,bool horizontalOnly = false) const;

    // returns a build location near a building's desired location
    BWAPI::TilePosition		getBuildLocationNear(const Building & b,int buildDist,bool horizontalOnly = false) const;

    void					reserveTiles(BWAPI::TilePosition position,int width,int height);
    void					freeTiles(BWAPI::TilePosition position,int width,int height);

    void					computeResourceBox();

    BWAPI::TilePosition		getRefineryPosition();
	int reserveWidth() const;
	int reserveHeight() const;

};
}