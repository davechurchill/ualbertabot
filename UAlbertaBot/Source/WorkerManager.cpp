#include "Common.h"
#include "WorkerManager.h"
#include "Micro.h"
#include "UnitUtil.h"
#include <BWAPI/Game.h>
#include <BWAPI/Player.h>
#include <BWAPI/Unitset.h>

using namespace UAlbertaBot;
using namespace AKBot;

WorkerManager::WorkerManager(
	shared_ptr<OpponentView> opponentView,
	shared_ptr<WorkerData> workerData)
	: _opponentView(opponentView)
	, workerData(workerData)
{
    previousClosestWorker = nullptr;
}

void WorkerManager::update(int currentFrame)
{
	updateWorkerStatus(currentFrame);
	handleGasWorkers(currentFrame);
	handleIdleWorkers(currentFrame);
	handleMoveWorkers(currentFrame);
	handleCombatWorkers(currentFrame);

    handleRepairWorkers(currentFrame);
}

void WorkerManager::updateWorkerStatus(int currentFrame)
{
	// for each of our Workers
	for (auto & worker : workerData->getWorkers())
	{
		if (!worker->isCompleted())
		{
			continue;
		}

		auto workerJob = workerData->getWorkerJob(worker);
		// if it's idle
		if (worker->isIdle() && 
			(workerJob != WorkerData::Build) && 
			(workerJob != WorkerData::Move) &&
			(workerJob != WorkerData::Scout)) 
		{
			workerData->setWorkerJob(worker, WorkerData::Idle, nullptr, currentFrame);
		}

		// if its job is gas
		if (workerJob == WorkerData::Gas)
		{
			BWAPI::Unit refinery = workerData->getWorkerResource(worker);

			// if the refinery doesn't exist anymore
			if (!refinery || !refinery->exists() ||	refinery->getHitPoints() <= 0)
			{
				setMineralWorker(worker, currentFrame);
			}
		}
	}
}

void WorkerManager::setRepairWorker(BWAPI::Unit worker, BWAPI::Unit unitToRepair, int currentFrame)
{
    workerData->setWorkerJob(worker, WorkerData::Repair, unitToRepair, currentFrame);
}

void WorkerManager::stopRepairing(BWAPI::Unit worker, int currentFrame)
{
    workerData->setWorkerJob(worker, WorkerData::Idle, nullptr, currentFrame);
}

void WorkerManager::handleGasWorkers(int currentFrame)
{
	// for each unit we have
	for (auto & unit : _opponentView->self()->getUnits())
	{
		// if that unit is a refinery
		if (unit->getType().isRefinery() && unit->isCompleted())
		{
			// get the number of workers currently assigned to it
			int numAssigned = workerData->getNumAssignedWorkers(unit);

			// if it's less than we want it to be, fill 'er up
			for (int i = 0; i < (_workersPerRefinery - numAssigned); ++i)
			{
				BWAPI::Unit gasWorker = getGasWorker(unit);
				if (gasWorker)
				{
					workerData->setWorkerJob(gasWorker, WorkerData::Gas, unit, currentFrame);
				}
			}
		}
	}
}

void WorkerManager::handleIdleWorkers(int currentFrame)
{
	// for each of our workers
	for (auto & worker : workerData->getWorkers())
	{
        UAB_ASSERT(worker != nullptr, "Worker was null");

		// if it is idle
		if (workerData->getWorkerJob(worker) == WorkerData::Idle)
		{
			// send it to the nearest mineral patch
			setMineralWorker(worker, currentFrame);
		}
	}
}

void WorkerManager::handleRepairWorkers(int currentFrame)
{
    if (_opponentView->self()->getRace() != BWAPI::Races::Terran)
    {
        return;
    }

    for (auto & unit : _opponentView->self()->getUnits())
    {
        if (unit->getType().isBuilding() && (unit->getHitPoints() < unit->getType().maxHitPoints()))
        {
            BWAPI::Unit repairWorker = getClosestMineralWorkerTo(unit);
            setRepairWorker(repairWorker, unit,currentFrame);
            break;
        }
    }
}

// bad micro for combat workers
void WorkerManager::handleCombatWorkers(int currentFrame)
{
	for (auto & worker : workerData->getWorkers())
	{
        UAB_ASSERT(worker != nullptr, "Worker was null");

		if (workerData->getWorkerJob(worker) == WorkerData::Combat)
		{
			//BWAPI::Broodwar->drawCircleMap(worker->getPosition().x, worker->getPosition().y, 4, BWAPI::Colors::Yellow, true);
			BWAPI::Unit target = getClosestEnemyUnit(worker);

			if (target)
			{
				Micro::SmartAttackUnit(worker, target, currentFrame);
			}
		}
	}
}

BWAPI::Unit WorkerManager::getClosestEnemyUnit(BWAPI::Unit worker)
{
    UAB_ASSERT(worker != nullptr, "Worker was null");

	BWAPI::Unit closestUnit = nullptr;
	double closestDist = 10000;

	for (auto & unit : UnitUtil::getEnemyUnits(_opponentView))
	{
		double dist = unit->getDistance(worker);

		if ((dist < 400) && (!closestUnit || (dist < closestDist)))
		{
			closestUnit = unit;
			closestDist = dist;
		}
	}

	return closestUnit;
}

void WorkerManager::finishedWithCombatWorkers(int currentFrame)
{
	for (auto & worker : workerData->getWorkers())
	{
        UAB_ASSERT(worker != nullptr, "Worker was null");

		if (workerData->getWorkerJob(worker) == WorkerData::Combat)
		{
			setMineralWorker(worker, currentFrame);
		}
	}
}

BWAPI::Unit WorkerManager::getClosestMineralWorkerTo(BWAPI::Unit enemyUnit)
{
    UAB_ASSERT(enemyUnit != nullptr, "enemyUnit was null");

    BWAPI::Unit closestMineralWorker = nullptr;
    double closestDist = 100000;

    if (previousClosestWorker)
    {
        if (previousClosestWorker->getHitPoints() > 0)
        {
            return previousClosestWorker;
        }
    }

    // for each of our workers
	for (auto & worker : workerData->getWorkers())
	{
        UAB_ASSERT(worker != nullptr, "Worker was null");
		if (!worker)
		{
			continue;
		}
		// if it is a move worker
        if (workerData->getWorkerJob(worker) == WorkerData::Minerals)
		{
			double dist = worker->getDistance(enemyUnit);

            if (!closestMineralWorker || dist < closestDist)
            {
                closestMineralWorker = worker;
                dist = closestDist;
            }
		}
	}

    previousClosestWorker = closestMineralWorker;
    return closestMineralWorker;
}


void WorkerManager::handleMoveWorkers(int currentFrame)
{
	// for each of our workers
	for (auto & worker : workerData->getWorkers())
	{
        UAB_ASSERT(worker != nullptr, "Worker was null");

		// if it is a move worker
		if (workerData->getWorkerJob(worker) == WorkerData::Move)
		{
			WorkerMoveData data = workerData->getWorkerMoveData(worker);
			
			Micro::SmartMove(worker, data.position, currentFrame);
		}
	}
}

// set a worker to mine minerals
void WorkerManager::setMineralWorker(BWAPI::Unit unit, int currentFrame)
{
    UAB_ASSERT(unit != nullptr, "Unit was null");

	// check if there is a mineral available to send the worker to
	BWAPI::Unit depot = getClosestDepot(unit);

	// if there is a valid mineral
	if (depot)
	{
		// update workerData with the new job
		workerData->setWorkerJob(unit, WorkerData::Minerals, depot, currentFrame);
	}
	else
	{
		// _logger.log("No valid depot for mineral worker");
	}
}

BWAPI::Unit WorkerManager::getClosestDepot(BWAPI::Unit worker)
{
	UAB_ASSERT(worker != nullptr, "Worker was null");

	BWAPI::Unit closestDepot = nullptr;
	double closestDistance = 0;

	for (auto & unit : _opponentView->self()->getUnits())
	{
        UAB_ASSERT(unit != nullptr, "Unit was null");

		if (unit->getType().isResourceDepot()
			&& (unit->isCompleted()
				|| unit->getType() == BWAPI::UnitTypes::Zerg_Lair
				|| unit->getType() == BWAPI::UnitTypes::Zerg_Hive)
			&& !workerData->depotIsFull(unit))
		{
			double distance = unit->getDistance(worker);
			if (!closestDepot || distance < closestDistance)
			{
				closestDepot = unit;
				closestDistance = distance;
			}
		}
	}

	return closestDepot;
}


// other managers that need workers call this when they're done with a unit
void WorkerManager::finishedWithWorker(BWAPI::Unit unit, int currentFrame)
{
	UAB_ASSERT(unit != nullptr, "Unit was null");

	//_logger.log("BuildingManager finished with worker %d", unit->getID());
	if (workerData->getWorkerJob(unit) != WorkerData::Scout)
	{
		workerData->setWorkerJob(unit, WorkerData::Idle, nullptr, currentFrame);
	}
}

BWAPI::Unit WorkerManager::getGasWorker(BWAPI::Unit refinery)
{
	UAB_ASSERT(refinery != nullptr, "Refinery was null");

	BWAPI::Unit closestWorker = nullptr;
	double closestDistance = 0;

	for (auto & unit : workerData->getWorkers())
	{
        UAB_ASSERT(unit != nullptr, "Unit was null");

		if (workerData->getWorkerJob(unit) == WorkerData::Minerals)
		{
			double distance = unit->getDistance(refinery);
			if (!closestWorker || distance < closestDistance)
			{
				closestWorker = unit;
				closestDistance = distance;
			}
		}
	}

	return closestWorker;
}

 void WorkerManager::setBuildingWorker(BWAPI::Unit worker, Building & b)
 {
     UAB_ASSERT(worker != nullptr, "Worker was null");

     workerData->setWorkerJob(worker, WorkerData::Build, b.type);
 }

// gets a builder for BuildingManager to use
// if setJobAsBuilder is true (default), it will be flagged as a builder unit
// set 'setJobAsBuilder' to false if we just want to see which worker will build a building
BWAPI::Unit WorkerManager::getBuilder(Building & b) const
{
	// variables to hold the closest worker of each type to the building
	BWAPI::Unit closestMovingWorker = nullptr;
	BWAPI::Unit closestMiningWorker = nullptr;
	double closestMovingWorkerDistance = 0;
	double closestMiningWorkerDistance = 0;

	// look through each worker that had moved there first
	for (auto & unit : workerData->getWorkers())
	{
        UAB_ASSERT(unit != nullptr, "Unit was null");
        
		// mining worker check
		if (unit->isCompleted() && (workerData->getWorkerJob(unit) == WorkerData::Minerals))
		{
			// if it is a new closest distance, set the pointer
			double distance = unit->getDistance(BWAPI::Position(b.finalPosition));
			if (!closestMiningWorker || distance < closestMiningWorkerDistance)
			{
				closestMiningWorker = unit;
				closestMiningWorkerDistance = distance;
			}
		}

		// moving worker check
		if (unit->isCompleted() && (workerData->getWorkerJob(unit) == WorkerData::Move))
		{
			// if it is a new closest distance, set the pointer
			double distance = unit->getDistance(BWAPI::Position(b.finalPosition));
			if (!closestMovingWorker || distance < closestMovingWorkerDistance)
			{
				closestMovingWorker = unit;
				closestMovingWorkerDistance = distance;
			}
		}
	}

	// if we found a moving worker, use it, otherwise using a mining worker
	BWAPI::Unit chosenWorker = closestMovingWorker ? closestMovingWorker : closestMiningWorker;

	// return the worker
	return chosenWorker;
}

// sets a worker as a scout
void WorkerManager::setScoutWorker(BWAPI::Unit worker, int currentFrame)
{
	UAB_ASSERT(worker != nullptr, "Worker was null");

	workerData->setWorkerJob(worker, WorkerData::Scout, nullptr, currentFrame);
}

// gets a worker which will move to a current location
BWAPI::Unit WorkerManager::getMoveWorker(BWAPI::Position p) const
{
	// set up the pointer
	BWAPI::Unit closestWorker = nullptr;
	double closestDistance = 0;

	// for each worker we currently have
	for (auto & unit : workerData->getWorkers())
	{
        UAB_ASSERT(unit != nullptr, "Unit was null");

		// only consider it if it's a mineral worker
		if (unit->isCompleted() && workerData->getWorkerJob(unit) == WorkerData::Minerals)
		{
			// if it is a new closest distance, set the pointer
			double distance = unit->getDistance(p);
			if (!closestWorker || distance < closestDistance)
			{
				closestWorker = unit;
				closestDistance = distance;
			}
		}
	}

	// return the worker
	return closestWorker;
}

// sets a worker to move to a given location
void WorkerManager::setMoveWorker(int mineralsNeeded, int gasNeeded, BWAPI::Position p)
{
	// set up the pointer
	BWAPI::Unit closestWorker = nullptr;
	double closestDistance = 0;

	// for each worker we currently have
	for (auto & unit : workerData->getWorkers())
	{
        UAB_ASSERT(unit != nullptr, "Unit was null");

		// only consider it if it's a mineral worker
		if (unit->isCompleted() && workerData->getWorkerJob(unit) == WorkerData::Minerals)
		{
			// if it is a new closest distance, set the pointer
			double distance = unit->getDistance(p);
			if (!closestWorker || distance < closestDistance)
			{
				closestWorker = unit;
				closestDistance = distance;
			}
		}
	}

	if (closestWorker)
	{
		//_logger.log("Setting worker job Move for worker %d", closestWorker->getID());
		workerData->setWorkerJob(closestWorker, WorkerData::Move, WorkerMoveData(mineralsNeeded, gasNeeded, p));
	}
	else
	{
		//_logger.log("Error, no worker found");
	}
}

// will we have the required resources by the time a worker can travel a certain distance
bool WorkerManager::willHaveResources(int mineralsRequired, int gasRequired, double distance) const
{
	// if we don't require anything, we will have it
	if (mineralsRequired <= 0 && gasRequired <= 0)
	{
		return true;
	}

	// the speed of the worker unit
	double speed = _opponentView->self()->getRace().getWorker().topSpeed();

    UAB_ASSERT(speed > 0, "Speed is negative");

	// how many frames it will take us to move to the building location
	// add a second to account for worker getting stuck. better early than late
	double framesToMove = (distance / speed) + 50;

	// magic numbers to predict income rates
	double mineralRate = getNumMineralWorkers() * 0.045;
	double gasRate     = getNumGasWorkers() * 0.07;

	// calculate if we will have enough by the time the worker gets there
	if (mineralRate * framesToMove >= mineralsRequired &&
		gasRate * framesToMove >= gasRequired)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void WorkerManager::setCombatWorker(BWAPI::Unit worker, int currentFrame)
{
	UAB_ASSERT(worker != nullptr, "Worker was null");

	workerData->setWorkerJob(worker, WorkerData::Combat, nullptr, currentFrame);
}

void WorkerManager::onUnitMorph(BWAPI::Unit unit)
{
	UAB_ASSERT(unit != nullptr, "Unit was null");

	auto isOwnedUnit = unit->getPlayer() == _opponentView->self();
	// if something morphs into a worker, add it
	if (unit->getType().isWorker() && isOwnedUnit && unit->getHitPoints() >= 0)
	{
		workerData->registerWorker(unit);
	}

	// if something morphs into a building, it was a worker?
	if (unit->getType().isBuilding() && isOwnedUnit && unit->getPlayer()->getRace() == BWAPI::Races::Zerg)
	{
		//_logger.log("A Drone started building");
		workerData->workerDestroyed(unit);
	}
}

void WorkerManager::onUnitShow(BWAPI::Unit unit)
{
	UAB_ASSERT(unit != nullptr, "Unit was null");

	// add the depot if it exists
	if (unit->getType().isResourceDepot() && unit->getPlayer() == _opponentView->self())
	{
		workerData->registerResourceDepot(unit);
	}

	// if something morphs into a worker, add it
	if (unit->getType().isWorker() && unit->getPlayer() == _opponentView->self() && unit->getHitPoints() >= 0)
	{
		//_logger.log("A worker was shown %d", unit->getID());
		workerData->registerWorker(unit);
	}
}

void WorkerManager::rebalanceWorkers(int currentFrame)
{
	// for each worker
	for (auto & worker : workerData->getWorkers())
	{
        UAB_ASSERT(worker != nullptr, "Worker was null");

		if (!workerData->getWorkerJob(worker) == WorkerData::Minerals)
		{
			continue;
		}

		BWAPI::Unit depot = workerData->getWorkerDepot(worker);

		if (depot && workerData->depotIsFull(depot))
		{
			workerData->setWorkerJob(worker, WorkerData::Idle, nullptr, currentFrame);
		}
		else if (!depot)
		{
			workerData->setWorkerJob(worker, WorkerData::Idle, nullptr, currentFrame);
		}
	}
}

void WorkerManager::onUnitDestroy(BWAPI::Unit unit, int currentFrame)
{
	UAB_ASSERT(unit != nullptr, "Unit was null");

	if (unit->getType().isResourceDepot() && unit->getPlayer() == _opponentView->self())
	{
		workerData->unregisterResourceDepot(unit, currentFrame);
	}

	if (unit->getType().isWorker() && unit->getPlayer() == _opponentView->self())
	{
		workerData->workerDestroyed(unit);
	}

	if (unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field)
	{
		rebalanceWorkers(currentFrame);
	}
}

bool WorkerManager::isFree(BWAPI::Unit worker) const
{
    UAB_ASSERT(worker != nullptr, "Worker was null");

	return workerData->getWorkerJob(worker) == WorkerData::Minerals || workerData->getWorkerJob(worker) == WorkerData::Idle;
}

bool WorkerManager::isWorkerScout(BWAPI::Unit worker) const
{
    UAB_ASSERT(worker != nullptr, "Worker was null");

	return (workerData->getWorkerJob(worker) == WorkerData::Scout);
}

bool WorkerManager::isBuilder(BWAPI::Unit worker) const
{
    UAB_ASSERT(worker != nullptr, "Worker was null");

	return (workerData->getWorkerJob(worker) == WorkerData::Build);
}

int WorkerManager::getNumMineralWorkers() const
{
	return workerData->getNumMineralWorkers();
}

int WorkerManager::getNumIdleWorkers() const
{
	return workerData->getNumIdleWorkers();
}

int WorkerManager::getNumGasWorkers() const
{
	return workerData->getNumGasWorkers();
}
