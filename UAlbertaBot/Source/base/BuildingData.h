#pragma once

#include "Common.h"

namespace UAlbertaBot
{
class Building {
     
public:
      
	BWAPI::TilePosition desiredPosition;
	BWAPI::TilePosition finalPosition;
	BWAPI::Position position;
	BWAPI::UnitType type;
	BWAPI::UnitInterface* buildingUnit;
	BWAPI::UnitInterface* builderUnit;
	int lastOrderFrame;
	bool buildCommandGiven;
	bool underConstruction;

	Building() 
		: desiredPosition(0,0)
        , finalPosition(BWAPI::TilePositions::None)
        , position(0,0)
        , type(BWAPI::UnitTypes::Unknown)
        , buildingUnit(NULL)
        , builderUnit(NULL)
        , lastOrderFrame(0)
        , buildCommandGiven(false)
        , underConstruction(false) 
    {} 

	// constructor we use most often
	Building(BWAPI::UnitType t, BWAPI::TilePosition desired)
		: desiredPosition(desired)
        , finalPosition(0,0)
        , position(0,0)
        , type(t)
        , buildingUnit(NULL)
        , builderUnit(NULL)
        , lastOrderFrame(0)
        , buildCommandGiven(false)
        , underConstruction(false) 
    {}

	// equals operator
	bool operator==(const Building & b) 
    {
		// buildings are equal if their worker unit or building unit are equal
		return (b.buildingUnit == buildingUnit) || (b.builderUnit == builderUnit);
	}
};

class ConstructionData 
{
public:

	typedef enum BuildingState_t {Unassigned = 0, Assigned = 1, UnderConstruction = 2, NumBuildingStates = 3} BuildingState;

private:

	std::vector< size_t >						buildingIndex;
	std::vector< std::vector<Building> >		buildings;			// buildings which do not yet have builders assigned

	std::set<BWAPI::UnitInterface*>		buildingUnitsConstructing;		// units which have been recently detected as started construction

public:

	ConstructionData();
	
	Building &					getNextBuilding(BuildingState bs);
	bool						hasNextBuilding(BuildingState bs);
	void						begin(BuildingState bs);
	void						addBuilding(BuildingState bs, const Building & b);
	void						removeCurrentBuilding(BuildingState bs);
	void						removeBuilding(BuildingState bs, Building & b);

	int							getNumBuildings(BuildingState bs);

	bool						isBeingBuilt(BWAPI::UnitType type);
};
}