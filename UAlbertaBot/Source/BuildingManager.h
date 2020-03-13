#pragma once

#include "Common.h"
#include "BuildingData.h"
#include "BuildingPlacerManager.h"

namespace UAlbertaBot
{

class BuildingManager
{
    BuildingPlacerManager m_buildingPlacer;
    std::vector<Building> m_buildings;

    bool m_debugMode        = false;
    int  m_reservedMinerals = 0;        // minerals reserved for planned buildings
    int  m_reservedGas      = 0;        // gas reserved for planned buildings

    bool isEvolvedBuilding(BWAPI::UnitType type);
    bool isBuildingPositionExplored(const Building & b) const;
    void removeBuildings(const std::vector<Building> & toRemove);

    void validateWorkersAndBuildings();		    // STEP 1
    void assignWorkersToUnassignedBuildings();	// STEP 2
    void constructAssignedBuildings();			// STEP 3
    void checkForStartedConstruction();			// STEP 4
    void checkForDeadTerranBuilders();			// STEP 5
    void checkForCompletedBuildings();			// STEP 6

    char getBuildingWorkerCode(const Building & b) const;
    

public:
    
    BuildingManager();

    void update();
    void addBuildingTask(BWAPI::UnitType type,BWAPI::TilePosition desiredLocation,bool isGasSteal);
    void drawBuildingInformation(int x,int y);
    int  getReservedMinerals();
    int  getReservedGas();
    bool isBeingBuilt(BWAPI::UnitType type);

    BWAPI::TilePosition getBuildingLocation(const Building & b);
    std::vector<BWAPI::UnitType> buildingsQueued();
};
}