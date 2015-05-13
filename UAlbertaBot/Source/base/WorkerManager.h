#pragma once

#include <Common.h>
#include "BuildingManager.h"
#include "WorkerData.h"

namespace UAlbertaBot
{
class Building;

class WorkerManager {

	//static WorkerManager *		instance;

	WorkerData					workerData;
    BWAPI::UnitInterface*               previousClosestWorker;

	int							workersPerRefinery;

	void						setMineralWorker(BWAPI::UnitInterface* unit);

	WorkerManager();

public:

	void						update();
	void						onUnitDestroy(BWAPI::UnitInterface* unit);
	void						onUnitMorph(BWAPI::UnitInterface* unit);
	void						onUnitShow(BWAPI::UnitInterface* unit);
	void						onUnitRenegade(BWAPI::UnitInterface* unit);
	void						finishedWithWorker(BWAPI::UnitInterface* unit);

	void						handleIdleWorkers();
	void						handleGasWorkers();
	void						handleMoveWorkers();
	void						handleCombatWorkers();
    void                        handleRepairWorkers();
	void						finishedWithCombatWorkers();

	void						drawResourceDebugInfo();
	void						updateWorkerStatus();
	void						drawWorkerInformation(int x, int y);

	int							getNumMineralWorkers();
	int							getNumGasWorkers();
	int							getNumIdleWorkers();
	void						setScoutWorker(BWAPI::UnitInterface* worker);

	bool						isWorkerScout(BWAPI::UnitInterface* worker);
	bool						isFree(BWAPI::UnitInterface* worker);
	bool						isBuilder(BWAPI::UnitInterface* worker);

	BWAPI::UnitInterface*				getBuilder(Building & b, bool setJobAsBuilder = true);
	BWAPI::UnitInterface*				getMoveWorker(BWAPI::Position p);
	BWAPI::UnitInterface*				getClosestDepot(BWAPI::UnitInterface* worker);
	BWAPI::UnitInterface*				getGasWorker(BWAPI::UnitInterface* refinery);
	BWAPI::UnitInterface*				getClosestEnemyUnit(BWAPI::UnitInterface* worker);
    BWAPI::UnitInterface*               getClosestMineralWorkerTo(BWAPI::UnitInterface* enemyUnit);

    void                        setBuildingWorker(BWAPI::UnitInterface* worker, Building & b);
    void                        setRepairWorker(BWAPI::UnitInterface* worker, BWAPI::UnitInterface* unitToRepair);
    void                        stopRepairing(BWAPI::UnitInterface* worker);
	void						setMoveWorker(int m, int g, BWAPI::Position p);
	void						setCombatWorker(BWAPI::UnitInterface* worker);
	void						smartAttackUnit(BWAPI::UnitInterface* attacker, BWAPI::UnitInterface* target);

	bool						willHaveResources(int mineralsRequired, int gasRequired, double distance);
	void						rebalanceWorkers();

	static WorkerManager &		Instance();

	const WorkerData &			getData() const;//returns a copy of the WorkerData
};
}