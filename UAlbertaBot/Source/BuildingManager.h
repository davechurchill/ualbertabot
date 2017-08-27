#pragma once

#include <Common.h>
#include "WorkerManager.h"
#include "BuildingPlacer.h"
#include "MapTools.h"
#include "BaseLocationManager.h"
#include "WorkerManager.h"
#include "Logger.h"
#include "BotConfiguration.h"

namespace UAlbertaBot
{
class BuildingManager
{
    BuildingPlacer  _buildingPlacer;
    std::vector<Building> _buildings;
	shared_ptr<AKBot::OpponentView> _opponentView;
	shared_ptr<BaseLocationManager> _bases;
	shared_ptr<WorkerManager> _workerManager;
	shared_ptr<MapTools> _mapTools;
	std::shared_ptr<AKBot::Logger> _logger;
	const BotMacroConfiguration& _macroConfiguration;

    bool            _debugMode;
    int             _reservedMinerals;				// minerals reserved for planned buildings
    int             _reservedGas;					// gas reserved for planned buildings

    bool            isEvolvedBuilding(BWAPI::UnitType type);
    bool            isBuildingPositionExplored(const Building & b) const;
    void            removeBuildings(const std::vector<Building> & toRemove);

    void            validateWorkersAndBuildings();		    // STEP 1
    void            assignWorkersToUnassignedBuildings();	// STEP 2
    void            constructAssignedBuildings(int currentFrame);			// STEP 3
    void            checkForStartedConstruction(int currentFrame);			// STEP 4
    void            checkForDeadTerranBuilders();			// STEP 5
    void            checkForCompletedBuildings(int currentFrame);			// STEP 6
              

public:
    
    BuildingManager(
		shared_ptr<AKBot::OpponentView> opponentView,
		shared_ptr<BaseLocationManager> bases,
		shared_ptr<WorkerManager> workerManager,
		shared_ptr<MapTools> mapTools,
		shared_ptr<AKBot::Logger> logger,
		const BotMacroConfiguration& macroConfiguration);

    void                update(int currentFrame);
    void                addBuildingTask(BWAPI::UnitType type,BWAPI::TilePosition desiredLocation);
    BWAPI::TilePosition getBuildingLocation(const Building & b);

    int                 getReservedMinerals();
    int                 getReservedGas();

    bool                isBeingBuilt(BWAPI::UnitType type);
	char            getBuildingWorkerCode(const Building & b) const;

    std::vector<BWAPI::Unit> & getFinishedWorkers();
    std::vector<BWAPI::Unit> & getFinishedScouts();

    std::vector<BWAPI::UnitType> buildingsQueued() const;
	const std::vector<Building>& getBuildings() const { return _buildings; }
	const BuildingPlacer& getBuildingPlacer() const { return _buildingPlacer; }
};
}