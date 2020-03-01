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
      
    BWAPI::TilePosition     desiredPosition     = {0, 0};
	BWAPI::TilePosition     finalPosition       = BWAPI::TilePositions::None;
    BWAPI::Position         position            = {0, 0};
	BWAPI::UnitType         type                = BWAPI::UnitTypes::Unknown;
	BWAPI::Unit             buildingUnit        = nullptr;
	BWAPI::Unit             builderUnit         = nullptr;
    size_t                  status              = BuildingStatus::Unassigned;
	int                     lastOrderFrame      = 0;
    bool                    isGasSteal          = false;     
	bool                    buildCommandGiven   = false;
	bool                    underConstruction   = false;

	Building() 
    {
    } 

	// constructor we use most often
	Building(BWAPI::UnitType t, BWAPI::TilePosition desired)
		: desiredPosition   (desired)
        , type              (t)
    {
    }

	// equals operator
	bool operator==(const Building & b) 
    {
		// buildings are equal if their worker unit or building unit are equal
		return (b.buildingUnit == buildingUnit) || (b.builderUnit == builderUnit);
	}
};

class BuildingData 
{
    std::vector<Building> m_buildings;

public:

	BuildingData();
	
    std::vector<Building> & getBuildings();

	void        addBuilding(const Building & b);
	void        removeBuilding(const Building & b);
    void        removeBuildings(const std::vector<Building> & buildings);
	bool        isBeingBuilt(BWAPI::UnitType type);
};
}