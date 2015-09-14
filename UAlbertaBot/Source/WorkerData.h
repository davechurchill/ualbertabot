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

	std::set<BWAPI::UnitInterface*>							workers;
	std::set<BWAPI::UnitInterface*>							depots;

	std::map<BWAPI::UnitInterface*, enum WorkerJob>			workerJobMap;
	std::map<BWAPI::UnitInterface*, BWAPI::UnitInterface*>			workerMineralMap;
	std::map<BWAPI::UnitInterface*, BWAPI::UnitInterface*>			workerDepotMap;
	std::map<BWAPI::UnitInterface*, BWAPI::UnitInterface*>			workerRefineryMap;
	std::map<BWAPI::UnitInterface*, BWAPI::UnitInterface*>			workerRepairMap;
	std::map<BWAPI::UnitInterface*, WorkerMoveData>			workerMoveMap;
	std::map<BWAPI::UnitInterface*, BWAPI::UnitType>		workerBuildingTypeMap;

	std::map<BWAPI::UnitInterface*, int>					depotWorkerCount;
	std::map<BWAPI::UnitInterface*, int>					refineryWorkerCount;

    std::map<BWAPI::UnitInterface*, int>					workersOnMineralPatch;
    std::map<BWAPI::UnitInterface*, BWAPI::UnitInterface*>			workerMineralAssignment;

	void											clearPreviousJob(BWAPI::UnitInterface* unit);

public:

	WorkerData();

	void					workerDestroyed(BWAPI::UnitInterface* unit);
	void					addDepot(BWAPI::UnitInterface* unit);
	void					removeDepot(BWAPI::UnitInterface* unit);
	void					addWorker(BWAPI::UnitInterface* unit);
	void					addWorker(BWAPI::UnitInterface* unit, WorkerJob job, BWAPI::UnitInterface* jobUnit);
	void					addWorker(BWAPI::UnitInterface* unit, WorkerJob job, BWAPI::UnitType jobUnitType);
	void					setWorkerJob(BWAPI::UnitInterface* unit, WorkerJob job, BWAPI::UnitInterface* jobUnit);
	void					setWorkerJob(BWAPI::UnitInterface* unit, WorkerJob job, WorkerMoveData wmd);
	void					setWorkerJob(BWAPI::UnitInterface* unit, WorkerJob job, BWAPI::UnitType jobUnitType);

	int						getNumWorkers() const;
	int						getNumMineralWorkers() const;
	int						getNumGasWorkers() const;
	int						getNumIdleWorkers() const;
	char					getJobCode(BWAPI::UnitInterface* unit);

	void					getMineralWorkers(std::set<BWAPI::UnitInterface*> & mw);
	void					getGasWorkers(std::set<BWAPI::UnitInterface*> & mw);
	void					getBuildingWorkers(std::set<BWAPI::UnitInterface*> & mw);
	void					getRepairWorkers(std::set<BWAPI::UnitInterface*> & mw);
	
	bool					depotIsFull(BWAPI::UnitInterface* depot);
	int						getMineralsNearDepot(BWAPI::UnitInterface* depot);

	int						getNumAssignedWorkers(BWAPI::UnitInterface* unit);
	BWAPI::UnitInterface*			getMineralToMine(BWAPI::UnitInterface* worker);

	enum WorkerJob			getWorkerJob(BWAPI::UnitInterface* unit);
	BWAPI::UnitInterface*			getWorkerResource(BWAPI::UnitInterface* unit);
	BWAPI::UnitInterface*			getWorkerDepot(BWAPI::UnitInterface* unit);
	BWAPI::UnitInterface*			getWorkerRepairUnit(BWAPI::UnitInterface* unit);
	BWAPI::UnitType			getWorkerBuildingType(BWAPI::UnitInterface* unit);
	WorkerMoveData			getWorkerMoveData(BWAPI::UnitInterface* unit);

    std::vector<BWAPI::UnitInterface*> getMineralPatchesNearDepot(BWAPI::UnitInterface* depot);
    void                    addToMineralPatch(BWAPI::UnitInterface* unit, int num);
	void					drawDepotDebugInfo();

	const std::set<BWAPI::UnitInterface*> & getWorkers() const { return workers; }

};
}