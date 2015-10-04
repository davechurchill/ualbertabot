#pragma once

#include "Common.h"

namespace UAlbertaBot
{

namespace BuildingStatus
{
    enum { Unassigned = 0, Assigned = 1, UnderConstruction = 2, Size = 3 };
}

class Building 
{     
public:
      
	BWAPI::TilePosition     desiredPosition;
	BWAPI::TilePosition     finalPosition;
	BWAPI::Position         position;
	BWAPI::UnitType         type;
	BWAPI::Unit             buildingUnit;
	BWAPI::Unit             builderUnit;
    size_t                  status;
	int                     lastOrderFrame;
    bool                    isGasSteal;
	bool                    buildCommandGiven;
	bool                    underConstruction;

	Building() 
		: desiredPosition   (0,0)
        , finalPosition     (BWAPI::TilePositions::None)
        , position          (0,0)
        , type              (BWAPI::UnitTypes::Unknown)
        , buildingUnit      (nullptr)
        , builderUnit       (nullptr)
        , lastOrderFrame    (0)
        , status            (BuildingStatus::Unassigned)
        , buildCommandGiven (false)
        , underConstruction (false) 
        , isGasSteal        (false)
    {} 

	// constructor we use most often
	Building(BWAPI::UnitType t, BWAPI::TilePosition desired)
		: desiredPosition   (desired)
        , finalPosition     (0,0)
        , position          (0,0)
        , type              (t)
        , buildingUnit      (nullptr)
        , builderUnit       (nullptr)
        , lastOrderFrame    (0)
        , status            (BuildingStatus::Unassigned)
        , buildCommandGiven (false)
        , underConstruction (false) 
        , isGasSteal        (false)
    {}

	// equals operator
	bool operator==(const Building & b) 
    {
		// buildings are equal if their worker unit or building unit are equal
		return (b.buildingUnit == buildingUnit) || (b.builderUnit == builderUnit);
	}
};

class BuildingData 
{
    std::vector<Building>                   _buildings;

public:

	BuildingData();
	
    std::vector<Building> & getBuildings();

	void        addBuilding(const Building & b);
	void        removeBuilding(const Building & b);
    void        removeBuildings(const std::vector<Building> & buildings);
	bool        isBeingBuilt(BWAPI::UnitType type);
};
}