#pragma once

#include <Common.h>
#include "WorkerManager.h"
#include "BuildingPlacer.h"
#include "InformationManager.h"
#include "MapTools.h"

namespace UAlbertaBot
{
class BuildingManager
{
    BuildingManager();

    std::vector<Building> _buildings;

    bool            _debugMode;
    int             _reservedMinerals;				// minerals reserved for planned buildings
    int             _reservedGas;					// gas reserved for planned buildings

    bool            isEvolvedBuilding(BWAPI::UnitType type);
    bool            isBuildingPositionExplored(const Building & b) const;
    void            removeBuildings(const std::vector<Building> & toRemove);

    void            validateWorkersAndBuildings();		    // STEP 1
    void            assignWorkersToUnassignedBuildings();	// STEP 2
    void            constructAssignedBuildings();			// STEP 3
    void            checkForStartedConstruction();			// STEP 4
    void            checkForDeadTerranBuilders();			// STEP 5
    void            checkForCompletedBuildings();			// STEP 6

    char            getBuildingWorkerCode(const Building & b) const;
    

public:
    
    static BuildingManager &	Instance();

    void                update();
    void                onUnitMorph(BWAPI::Unit unit);
    void                onUnitDestroy(BWAPI::Unit unit);
    void                addBuildingTask(BWAPI::UnitType type,BWAPI::TilePosition desiredLocation,bool isGasSteal);
    void                drawBuildingInformation(int x,int y);
    BWAPI::TilePosition getBuildingLocation(const Building & b);

    int                 getReservedMinerals();
    int                 getReservedGas();

    bool                isBeingBuilt(BWAPI::UnitType type);

    std::vector<BWAPI::UnitType> buildingsQueued();
};
}