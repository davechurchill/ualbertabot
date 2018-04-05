#pragma once

#include "Common.h"
#include <BWAPI/Unit.h>
#include <BWAPI/UnitType.h>
#include "Logger.h"
#include "UnitInformation.h"

namespace UAlbertaBot
{
class WorkerMoveData
{
public:

    int mineralsNeeded;
    int gasNeeded;
    BWAPI::Position position;

    WorkerMoveData(int m,int g,BWAPI::Position p)
        : mineralsNeeded(m)
        ,gasNeeded(g)
        ,position(p)
    {

    }

    WorkerMoveData() {}
};

class WorkerData
{

public:

    enum WorkerJob {
		Minerals,
		Gas,
		Build,
		Combat,
		Idle,
		Repair,
		Move,
		Scout,
		Default
	};

private:

    std::set<BWAPI::Unit>                   _workers;
    std::set<BWAPI::Unit>                   _depots;

    std::map<BWAPI::Unit,enum WorkerJob>    _workerJobMap;
    std::map<BWAPI::Unit,BWAPI::Unit>       _workerMineralMap;
    std::map<BWAPI::Unit,BWAPI::Unit>       _workerDepotMap;
    std::map<BWAPI::Unit,BWAPI::Unit>       _workerRefineryMap;
    std::map<BWAPI::Unit,BWAPI::Unit>       _workerRepairMap;
    std::map<BWAPI::Unit,WorkerMoveData>    _workerMoveMap;
    std::map<BWAPI::Unit,BWAPI::UnitType>   _workerBuildingTypeMap;

    std::map<BWAPI::Unit,int>               _depotWorkerCount;
    std::map<BWAPI::Unit,int>               _refineryWorkerCount;

    std::map<BWAPI::Unit,int>               _workersOnMineralPatch;
	shared_ptr<AKBot::Logger> _logger;
	shared_ptr<AKBot::UnitInformation> _unitInformation;

    void clearPreviousJob(BWAPI::Unit unit);

public:

    WorkerData(shared_ptr<AKBot::Logger> logger, shared_ptr<AKBot::UnitInformation> unitInformation);

    void    workerDestroyed(BWAPI::Unit unit);
	/* Register resource depot */
    void    registerResourceDepot(BWAPI::Unit resourceDepot);
	/* Unregister resource depot */
    void    unregisterResourceDepot(BWAPI::Unit resourceDepot, int currentFrame);
    void    registerWorker(BWAPI::Unit unit);
    void    addWorker(BWAPI::Unit unit,WorkerJob job,BWAPI::Unit jobUnit, int currentFrame);
    void    addWorker(BWAPI::Unit unit,WorkerJob job,BWAPI::UnitType jobUnitType);
    void    setWorkerJob(BWAPI::Unit unit,WorkerJob job,BWAPI::Unit jobUnit, int currentFrame);
    void    setWorkerJob(BWAPI::Unit unit,WorkerJob job,WorkerMoveData wmd);
    void    setWorkerJob(BWAPI::Unit unit,WorkerJob job,BWAPI::UnitType jobUnitType);
    void    addToMineralPatch(BWAPI::Unit unit,int num);

    int     getNumWorkers() const;
    int     getNumMineralWorkers() const;
    int     getNumGasWorkers() const;
    int     getNumIdleWorkers() const;
    char    getJobCode(BWAPI::Unit unit) const;

    void    getMineralWorkers(std::set<BWAPI::Unit> & mw);
    void    getGasWorkers(std::set<BWAPI::Unit> & mw);
    void    getBuildingWorkers(std::set<BWAPI::Unit> & mw);
    void    getRepairWorkers(std::set<BWAPI::Unit> & mw);

    bool    depotIsFull(BWAPI::Unit depot);
    int     getMineralsNearDepot(BWAPI::Unit depot);

    int     getNumAssignedWorkers(BWAPI::Unit unit) const;
    BWAPI::Unit getMineralToMine(BWAPI::Unit worker);

    enum WorkerJob  getWorkerJob(BWAPI::Unit unit) const;
    BWAPI::Unit     getWorkerResource(BWAPI::Unit unit) const;
    BWAPI::Unit     getWorkerDepot(BWAPI::Unit unit) const;
    BWAPI::Unit     getWorkerRepairUnit(BWAPI::Unit unit) const;
    BWAPI::UnitType getWorkerBuildingType(BWAPI::Unit unit) const;
    WorkerMoveData  getWorkerMoveData(BWAPI::Unit unit) const;

    std::vector<BWAPI::Unit>    getMineralPatchesNearDepot(BWAPI::Unit depot) const;

    const std::set<BWAPI::Unit> & getWorkers() const;
	const std::set<BWAPI::Unit>& getDepots() const;
	const std::map<BWAPI::Unit, int>& getWorkersOnMineralPatch() const;
};
}