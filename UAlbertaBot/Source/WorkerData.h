#pragma once

#include "Common.h"

namespace UAlbertaBot
{
class WorkerMoveData
{
public:

	int mineralsNeeded;
	int gasNeeded;
	BWAPI::Position position;

	WorkerMoveData(int m, int g, BWAPI::Position p)
        : mineralsNeeded(m)
        , gasNeeded(g)
        , position(p)
	{
		
	}

	WorkerMoveData() {}
};

class WorkerData 
{

public:

	enum WorkerJob {Minerals, Gas, Build, Combat, Idle, Repair, Move, Scout, Default};

private:

	BWAPI::Unitset workers;
	BWAPI::Unitset depots;

	std::map<BWAPI::Unit, enum WorkerJob>         workerJobMap;
	std::map<BWAPI::Unit, BWAPI::Unit>  workerMineralMap;
	std::map<BWAPI::Unit, BWAPI::Unit>  workerDepotMap;
	std::map<BWAPI::Unit, BWAPI::Unit>  workerRefineryMap;
	std::map<BWAPI::Unit, BWAPI::Unit>  workerRepairMap;
	std::map<BWAPI::Unit, WorkerMoveData>         workerMoveMap;
	std::map<BWAPI::Unit, BWAPI::UnitType>        workerBuildingTypeMap;

	std::map<BWAPI::Unit, int>                    depotWorkerCount;
	std::map<BWAPI::Unit, int>                    refineryWorkerCount;

    std::map<BWAPI::Unit, int>                    workersOnMineralPatch;
    std::map<BWAPI::Unit, BWAPI::Unit>  workerMineralAssignment;

	void clearPreviousJob(BWAPI::Unit unit);

public:

	WorkerData();

	void					workerDestroyed(BWAPI::Unit unit);
	void					addDepot(BWAPI::Unit unit);
	void					removeDepot(BWAPI::Unit unit);
	void					addWorker(BWAPI::Unit unit);
	void					addWorker(BWAPI::Unit unit, WorkerJob job, BWAPI::Unit jobUnit);
	void					addWorker(BWAPI::Unit unit, WorkerJob job, BWAPI::UnitType jobUnitType);
	void					setWorkerJob(BWAPI::Unit unit, WorkerJob job, BWAPI::Unit jobUnit);
	void					setWorkerJob(BWAPI::Unit unit, WorkerJob job, WorkerMoveData wmd);
	void					setWorkerJob(BWAPI::Unit unit, WorkerJob job, BWAPI::UnitType jobUnitType);

	int						getNumWorkers() const;
	int						getNumMineralWorkers() const;
	int						getNumGasWorkers() const;
	int						getNumIdleWorkers() const;
	char					getJobCode(BWAPI::Unit unit);

	void					getMineralWorkers(std::set<BWAPI::Unit> & mw);
	void					getGasWorkers(std::set<BWAPI::Unit> & mw);
	void					getBuildingWorkers(std::set<BWAPI::Unit> & mw);
	void					getRepairWorkers(std::set<BWAPI::Unit> & mw);
	
	bool					depotIsFull(BWAPI::Unit depot);
	int						getMineralsNearDepot(BWAPI::Unit depot);

	int						getNumAssignedWorkers(BWAPI::Unit unit);
	BWAPI::Unit   getMineralToMine(BWAPI::Unit worker);

	enum WorkerJob			getWorkerJob(BWAPI::Unit unit);
	BWAPI::Unit   getWorkerResource(BWAPI::Unit unit);
	BWAPI::Unit   getWorkerDepot(BWAPI::Unit unit);
	BWAPI::Unit   getWorkerRepairUnit(BWAPI::Unit unit);
	BWAPI::UnitType			getWorkerBuildingType(BWAPI::Unit unit);
	WorkerMoveData			getWorkerMoveData(BWAPI::Unit unit);

    BWAPI::Unitset          getMineralPatchesNearDepot(BWAPI::Unit depot);
    void                    addToMineralPatch(BWAPI::Unit unit, int num);
	void					drawDepotDebugInfo();

	const BWAPI::Unitset & getWorkers() const { return workers; }

};
}