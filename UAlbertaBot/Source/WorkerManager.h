#pragma once

#include <Common.h>
#include "BuildingData.h"
#include "WorkerData.h"
#include "OpponentView.h"

namespace UAlbertaBot
{
	using AKBot::OpponentView;

class WorkerManager
{
	shared_ptr<WorkerData>  workerData;
    BWAPI::Unit previousClosestWorker;
	std::shared_ptr<OpponentView> _opponentView;
	int _workersPerRefinery = 3;

    void        setMineralWorker(BWAPI::Unit unit, int currentFrame);
    
    void        handleIdleWorkers(int currentFrame);
    void        handleGasWorkers(int currentFrame);
    void        handleMoveWorkers(int currentFrame);
    void        handleCombatWorkers(int currentFrame);
    void        handleRepairWorkers(int currentFrame);

public:

    WorkerManager(
		shared_ptr<OpponentView> opponentView,
		shared_ptr<WorkerData>  workerData);

    void        update(int currentFrame);
    void        onUnitDestroy(BWAPI::Unit unit, int currentFrame);
    void        onUnitMorph(BWAPI::Unit unit);
    void        onUnitShow(BWAPI::Unit unit);
    void        finishedWithWorker(BWAPI::Unit unit, int currentFrame);

    void        finishedWithCombatWorkers(int currentFrame);

    void        updateWorkerStatus(int currentFrame);
	shared_ptr<WorkerData> getWorkerData() const { return workerData; };

    int         getNumMineralWorkers() const;
    int         getNumGasWorkers() const;
    int         getNumIdleWorkers() const;
    void        setScoutWorker(BWAPI::Unit worker, int currentFrame);

    bool        isWorkerScout(BWAPI::Unit worker) const;
    bool        isFree(BWAPI::Unit worker) const;
    bool        isBuilder(BWAPI::Unit worker) const;

    BWAPI::Unit getBuilder(Building & b) const;
    BWAPI::Unit getMoveWorker(BWAPI::Position p) const;
    BWAPI::Unit getClosestDepot(BWAPI::Unit worker);
    BWAPI::Unit getGasWorker(BWAPI::Unit refinery);
    BWAPI::Unit getClosestEnemyUnit(BWAPI::Unit worker);
    BWAPI::Unit getClosestMineralWorkerTo(BWAPI::Unit enemyUnit);

    void        setBuildingWorker(BWAPI::Unit worker,Building & b);
    void        setRepairWorker(BWAPI::Unit worker,BWAPI::Unit unitToRepair, int currentFrame);
    void        stopRepairing(BWAPI::Unit worker, int currentFrame);
    void        setMoveWorker(int m,int g,BWAPI::Position p);
    void        setCombatWorker(BWAPI::Unit worker, int currentFrame);

    bool        willHaveResources(int mineralsRequired,int gasRequired,double distance) const;
    void        rebalanceWorkers(int currentFrame);
	void		setWorkersPerRefinery(int value) { _workersPerRefinery = value; }
};

}