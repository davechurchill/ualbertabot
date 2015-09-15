#include "Common.h"
#include "WorkerManager.h"

using namespace UAlbertaBot;

WorkerManager::WorkerManager() 
    : workersPerRefinery(3) 
{
    previousClosestWorker = NULL;
}

void WorkerManager::update() 
{
	// worker bookkeeping
	updateWorkerStatus();

	// set the gas workers
	handleGasWorkers();

	// handle idle workers
	handleIdleWorkers();

	// handle move workers
	handleMoveWorkers();

	// handle combat workers
	handleCombatWorkers();

	drawResourceDebugInfo();
	drawWorkerInformation(450,20);

	workerData.drawDepotDebugInfo();

    handleRepairWorkers();
}

void WorkerManager::updateWorkerStatus() 
{
	// for each of our Workers
	for (BWAPI::UnitInterface* worker : workerData.getWorkers())
	{
		if (!worker->isCompleted())
		{
			continue;
		}

		// if it's idle
		if (worker->isIdle() && 
			(workerData.getWorkerJob(worker) != WorkerData::Build) && 
			(workerData.getWorkerJob(worker) != WorkerData::Move) &&
			(workerData.getWorkerJob(worker) != WorkerData::Scout)) 
		{
			//printf("Worker %d set to idle", worker->getID());
			// set its job to idle
			workerData.setWorkerJob(worker, WorkerData::Idle, NULL);
		}

		// if its job is gas
		if (workerData.getWorkerJob(worker) == WorkerData::Gas)
		{
			BWAPI::UnitInterface* refinery = workerData.getWorkerResource(worker);

			// if the refinery doesn't exist anymore
			if (!refinery || !refinery->exists() ||	refinery->getHitPoints() <= 0)
			{
				setMineralWorker(worker);
			}
		}
	}
}

void WorkerManager::setRepairWorker(BWAPI::UnitInterface* worker, BWAPI::UnitInterface* unitToRepair)
{
    workerData.setWorkerJob(worker, WorkerData::Repair, unitToRepair);
}

void WorkerManager::stopRepairing(BWAPI::UnitInterface* worker)
{
    workerData.setWorkerJob(worker, WorkerData::Idle, NULL);
}

void WorkerManager::handleGasWorkers() 
{
	// for each unit we have
	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->self()->getUnits())
	{
		// if that unit is a refinery
		if (unit->getType().isRefinery() && unit->isCompleted())
		{
			// get the number of workers currently assigned to it
			int numAssigned = workerData.getNumAssignedWorkers(unit);

			// if it's less than we want it to be, fill 'er up
			for (int i=0; i<(workersPerRefinery-numAssigned); ++i)
			{
				BWAPI::UnitInterface* gasWorker = getGasWorker(unit);
				if (gasWorker)
				{
					workerData.setWorkerJob(gasWorker, WorkerData::Gas, unit);
				}
			}
		}
	}

}

void WorkerManager::handleIdleWorkers() 
{
	// for each of our workers
	for (BWAPI::UnitInterface* worker : workerData.getWorkers())
	{
        UAB_ASSERT(worker != NULL, "Worker was null");

		// if it is idle
		if (workerData.getWorkerJob(worker) == WorkerData::Idle) 
		{
			// send it to the nearest mineral patch
			setMineralWorker(worker);
		}
	}
}

void WorkerManager::handleRepairWorkers()
{
    if (BWAPI::Broodwar->self()->getRace() != BWAPI::Races::Terran)
    {
        return;
    }

    for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->self()->getUnits())
    {
        if (unit->getType().isBuilding() && (unit->getHitPoints() < unit->getType().maxHitPoints()))
        {
            BWAPI::UnitInterface* repairWorker = getClosestMineralWorkerTo(unit);
            setRepairWorker(repairWorker, unit);
            break;
        }
    }
}

// bad micro for combat workers
void WorkerManager::handleCombatWorkers()
{
	for (BWAPI::UnitInterface* worker : workerData.getWorkers())
	{
        UAB_ASSERT(worker != NULL, "Worker was null");

		if (workerData.getWorkerJob(worker) == WorkerData::Combat)
		{
			BWAPI::Broodwar->drawCircleMap(worker->getPosition().x, worker->getPosition().y, 4, BWAPI::Colors::Yellow, true);
			BWAPI::UnitInterface* target = getClosestEnemyUnit(worker);

			if (target)
			{
				smartAttackUnit(worker, target);
			}
		}
	}
}

BWAPI::UnitInterface* WorkerManager::getClosestEnemyUnit(BWAPI::UnitInterface* worker)
{
    UAB_ASSERT(worker != NULL, "Worker was null");

	BWAPI::UnitInterface* closestUnit = NULL;
	double closestDist = 10000;

	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->enemy()->getUnits())
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

void WorkerManager::finishedWithCombatWorkers()
{
	for (BWAPI::UnitInterface* worker : workerData.getWorkers())
	{
        UAB_ASSERT(worker != NULL, "Worker was null");

		if (workerData.getWorkerJob(worker) == WorkerData::Combat)
		{
			setMineralWorker(worker);
		}
	}
}

BWAPI::UnitInterface* WorkerManager::getClosestMineralWorkerTo(BWAPI::UnitInterface* enemyUnit)
{
    UAB_ASSERT(enemyUnit != NULL, "enemyUnit was null");

    BWAPI::UnitInterface* closestMineralWorker = NULL;
    double closestDist = 100000;

    if (previousClosestWorker)
    {
        if (previousClosestWorker->getHitPoints() > 0)
        {
            return previousClosestWorker;
        }
    }

    // for each of our workers
	for (BWAPI::UnitInterface* worker : workerData.getWorkers())
	{
        UAB_ASSERT(worker != NULL, "Worker was null");
		if (!worker)
		{
			continue;
		}
		// if it is a move worker
        if (workerData.getWorkerJob(worker) == WorkerData::Minerals) 
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

void WorkerManager::handleMoveWorkers() 
{
	// for each of our workers
	for (BWAPI::UnitInterface* worker : workerData.getWorkers())
	{
        UAB_ASSERT(worker != NULL, "Worker was null");

		// if it is a move worker
		if (workerData.getWorkerJob(worker) == WorkerData::Move) 
		{
			WorkerMoveData data = workerData.getWorkerMoveData(worker);
			
			worker->move(data.position);
		}
	}
}

// set a worker to mine minerals
void WorkerManager::setMineralWorker(BWAPI::UnitInterface* unit)
{
    UAB_ASSERT(unit != NULL, "Unit was null");

	// check if there is a mineral available to send the worker to
	BWAPI::UnitInterface* depot = getClosestDepot(unit);

	// if there is a valid mineral
	if (depot)
	{
		// update workerData with the new job
		workerData.setWorkerJob(unit, WorkerData::Minerals, depot);
	}
	else
	{
		// BWAPI::Broodwar->printf("No valid depot for mineral worker");
	}
}

BWAPI::UnitInterface* WorkerManager::getClosestDepot(BWAPI::UnitInterface* worker)
{
	UAB_ASSERT(worker != NULL, "Worker was null");

	BWAPI::UnitInterface* closestDepot = NULL;
	double closestDistance = 0;

	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->self()->getUnits())
	{
        UAB_ASSERT(unit != NULL, "Unit was null");

		if (unit->getType().isResourceDepot() && unit->isCompleted() && !workerData.depotIsFull(unit))
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
void WorkerManager::finishedWithWorker(BWAPI::UnitInterface* unit) 
{
	UAB_ASSERT(unit != NULL, "Unit was null");

	//BWAPI::Broodwar->printf("BuildingManager finished with worker %d", unit->getID());
	if (workerData.getWorkerJob(unit) != WorkerData::Scout)
	{
		workerData.setWorkerJob(unit, WorkerData::Idle, NULL);
	}
}

BWAPI::UnitInterface* WorkerManager::getGasWorker(BWAPI::UnitInterface* refinery)
{
	UAB_ASSERT(refinery != NULL, "Refinery was null");

	BWAPI::UnitInterface* closestWorker = NULL;
	double closestDistance = 0;

	for (BWAPI::UnitInterface* unit : workerData.getWorkers())
	{
        UAB_ASSERT(unit != NULL, "Unit was null");

		if (workerData.getWorkerJob(unit) == WorkerData::Minerals)
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

 void WorkerManager::setBuildingWorker(BWAPI::UnitInterface* worker, Building & b)
 {
     UAB_ASSERT(worker != NULL, "Worker was null");

     workerData.setWorkerJob(worker, WorkerData::Build, b.type);
 }

// gets a builder for BuildingManager to use
// if setJobAsBuilder is true (default), it will be flagged as a builder unit
// set 'setJobAsBuilder' to false if we just want to see which worker will build a building
BWAPI::UnitInterface* WorkerManager::getBuilder(Building & b, bool setJobAsBuilder)
{
	// variables to hold the closest worker of each type to the building
	BWAPI::UnitInterface* closestMovingWorker = NULL;
	BWAPI::UnitInterface* closestMiningWorker = NULL;
	double closestMovingWorkerDistance = 0;
	double closestMiningWorkerDistance = 0;

	// look through each worker that had moved there first
	for (BWAPI::UnitInterface* unit : workerData.getWorkers())
	{
        UAB_ASSERT(unit != NULL, "Unit was null");

		// mining worker check
		if (unit->isCompleted() && (workerData.getWorkerJob(unit) == WorkerData::Minerals))
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
		if (unit->isCompleted() && (workerData.getWorkerJob(unit) == WorkerData::Move))
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
	BWAPI::UnitInterface* chosenWorker = closestMovingWorker ? closestMovingWorker : closestMiningWorker;

	// if the worker exists (one may not have been found in rare cases)
	if (chosenWorker && setJobAsBuilder)
	{
		workerData.setWorkerJob(chosenWorker, WorkerData::Build, b.type);
	}

	// return the worker
	return chosenWorker;
}

// sets a worker as a scout
void WorkerManager::setScoutWorker(BWAPI::UnitInterface* worker)
{
	UAB_ASSERT(worker != NULL, "Worker was null");

	workerData.setWorkerJob(worker, WorkerData::Scout, NULL);
}

// gets a worker which will move to a current location
BWAPI::UnitInterface* WorkerManager::getMoveWorker(BWAPI::Position p)
{
	// set up the pointer
	BWAPI::UnitInterface* closestWorker = NULL;
	double closestDistance = 0;

	// for each worker we currently have
	for (BWAPI::UnitInterface* unit : workerData.getWorkers())
	{
        UAB_ASSERT(unit != NULL, "Unit was null");

		// only consider it if it's a mineral worker
		if (unit->isCompleted() && workerData.getWorkerJob(unit) == WorkerData::Minerals)
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
	BWAPI::UnitInterface* closestWorker = NULL;
	double closestDistance = 0;

	// for each worker we currently have
	for (BWAPI::UnitInterface* unit : workerData.getWorkers())
	{
        UAB_ASSERT(unit != NULL, "Unit was null");

		// only consider it if it's a mineral worker
		if (unit->isCompleted() && workerData.getWorkerJob(unit) == WorkerData::Minerals)
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
		//BWAPI::Broodwar->printf("Setting worker job Move for worker %d", closestWorker->getID());
		workerData.setWorkerJob(closestWorker, WorkerData::Move, WorkerMoveData(mineralsNeeded, gasNeeded, p));
	}
	else
	{
		//BWAPI::Broodwar->printf("Error, no worker found");
	}
}

// will we have the required resources by the time a worker can travel a certain distance
bool WorkerManager::willHaveResources(int mineralsRequired, int gasRequired, double distance)
{
	// if we don't require anything, we will have it
	if (mineralsRequired <= 0 && gasRequired <= 0)
	{
		return true;
	}

	// the speed of the worker unit
	double speed = BWAPI::Broodwar->self()->getRace().getWorker().topSpeed();

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

void WorkerManager::setCombatWorker(BWAPI::UnitInterface* worker)
{
	UAB_ASSERT(worker != NULL, "Worker was null");

	workerData.setWorkerJob(worker, WorkerData::Combat, NULL);
}

void WorkerManager::onUnitMorph(BWAPI::UnitInterface* unit)
{
	UAB_ASSERT(unit != NULL, "Unit was null");

	// if something morphs into a worker, add it
	if (unit->getType().isWorker() && unit->getPlayer() == BWAPI::Broodwar->self() && unit->getHitPoints() >= 0)
	{
		workerData.addWorker(unit);
	}

	// if something morphs into a building, it was a worker?
	if (unit->getType().isBuilding() && unit->getPlayer() == BWAPI::Broodwar->self() && unit->getPlayer()->getRace() == BWAPI::Races::Zerg)
	{
		//BWAPI::Broodwar->printf("A Drone started building");
		workerData.workerDestroyed(unit);
	}
}

void WorkerManager::onUnitShow(BWAPI::UnitInterface* unit)
{
	UAB_ASSERT(unit != NULL, "Unit was null");

	// add the depot if it exists
	if (unit->getType().isResourceDepot() && unit->getPlayer() == BWAPI::Broodwar->self())
	{
		workerData.addDepot(unit);
	}

	// if something morphs into a worker, add it
	if (unit->getType().isWorker() && unit->getPlayer() == BWAPI::Broodwar->self() && unit->getHitPoints() >= 0)
	{
		//BWAPI::Broodwar->printf("A worker was shown %d", unit->getID());
		workerData.addWorker(unit);
	}
}


void WorkerManager::rebalanceWorkers()
{
	// for each worker
	for (BWAPI::UnitInterface* worker : workerData.getWorkers())
	{
        UAB_ASSERT(worker != NULL, "Worker was null");

		// we only care to rebalance mineral workers
		if (!workerData.getWorkerJob(worker) == WorkerData::Minerals)
		{
			continue;
		}

		// get the depot this worker works for
		BWAPI::UnitInterface* depot = workerData.getWorkerDepot(worker);

		// if there is a depot and it's full
		if (depot && workerData.depotIsFull(depot))
		{
			// set the worker to idle
			workerData.setWorkerJob(worker, WorkerData::Idle, NULL);
		}
		// if there's no depot
		else if (!depot)
		{
			// set the worker to idle
			workerData.setWorkerJob(worker, WorkerData::Idle, NULL);
		}
	}
}

void WorkerManager::onUnitDestroy(BWAPI::UnitInterface* unit) 
{
	UAB_ASSERT(unit != NULL, "Unit was null");

	// remove the depot if it exists
	if (unit->getType().isResourceDepot() && unit->getPlayer() == BWAPI::Broodwar->self())
	{
		workerData.removeDepot(unit);
	}

	// if the unit that was destroyed is a worker
	if (unit->getType().isWorker() && unit->getPlayer() == BWAPI::Broodwar->self()) 
	{
		// tell the worker data it was destroyed
		workerData.workerDestroyed(unit);
	}

	if (unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field)
	{
		//BWAPI::Broodwar->printf("A mineral died, rebalancing workers");

		rebalanceWorkers();
	}
}

void WorkerManager::smartAttackUnit(BWAPI::UnitInterface* attacker, BWAPI::UnitInterface* target)
{
    UAB_ASSERT(attacker != NULL, "Attacker was null");
    UAB_ASSERT(target != NULL, "Target was null");

	// if we have issued a command to this unit already this frame, ignore this one
	if (attacker->getLastCommandFrame() >= BWAPI::Broodwar->getFrameCount() || attacker->isAttackFrame())
	{
		return;
	}

	// get the unit's current command
	BWAPI::UnitCommand currentCommand(attacker->getLastCommand());

	// if we've already told this unit to attack this target, ignore this command
	if (currentCommand.getType() == BWAPI::UnitCommandTypes::Attack_Unit &&	currentCommand.getTarget() == target)
	{
		return;
	}

	// if nothing prevents it, attack the target
	attacker->attack(target);
}

void WorkerManager::drawResourceDebugInfo() 
{
    if (!Config::Debug::DrawResourceInfo)
    {
        return;
    }

	for (BWAPI::UnitInterface* worker : workerData.getWorkers()) 
    {
        UAB_ASSERT(worker != NULL, "Worker was null");

		char job = workerData.getJobCode(worker);

		BWAPI::Position pos = worker->getTargetPosition();

		BWAPI::Broodwar->drawTextMap(worker->getPosition().x, worker->getPosition().y - 5, "\x07%c", job);

		BWAPI::Broodwar->drawLineMap(worker->getPosition().x, worker->getPosition().y, pos.x, pos.y, BWAPI::Colors::Cyan);

		BWAPI::UnitInterface* depot = workerData.getWorkerDepot(worker);
		if (depot)
		{
			BWAPI::Broodwar->drawLineMap(worker->getPosition().x, worker->getPosition().y, depot->getPosition().x, depot->getPosition().y, BWAPI::Colors::Orange);
		}
	}
}

void WorkerManager::drawWorkerInformation(int x, int y) 
{
    if (!Config::Debug::DrawWorkerInfo)
    {
        return;
    }

	BWAPI::Broodwar->drawTextScreen(x, y, "\x04 Workers %d", workerData.getNumMineralWorkers());
	BWAPI::Broodwar->drawTextScreen(x, y+20, "\x04 UnitID");
	BWAPI::Broodwar->drawTextScreen(x+50, y+20, "\x04 State");

	int yspace = 0;

	for (BWAPI::UnitInterface* unit : workerData.getWorkers())
	{
        UAB_ASSERT(unit != NULL, "Worker was null");

		BWAPI::Broodwar->drawTextScreen(x, y+40+((yspace)*10), "\x03 %d", unit->getID());
		BWAPI::Broodwar->drawTextScreen(x+50, y+40+((yspace++)*10), "\x03 %c", workerData.getJobCode(unit));
	}
}

bool WorkerManager::isFree(BWAPI::UnitInterface* worker)
{
    UAB_ASSERT(worker != NULL, "Worker was null");

	return workerData.getWorkerJob(worker) == WorkerData::Minerals || workerData.getWorkerJob(worker) == WorkerData::Idle;
}

bool WorkerManager::isWorkerScout(BWAPI::UnitInterface* worker)
{
    UAB_ASSERT(worker != NULL, "Worker was null");

	return (workerData.getWorkerJob(worker) == WorkerData::Scout);
}

bool WorkerManager::isBuilder(BWAPI::UnitInterface* worker)
{
    UAB_ASSERT(worker != NULL, "Worker was null");

	return (workerData.getWorkerJob(worker) == WorkerData::Build);
}

int WorkerManager::getNumMineralWorkers() 
{
	return workerData.getNumMineralWorkers();	
}

int WorkerManager::getNumIdleWorkers() 
{
	return workerData.getNumIdleWorkers();	
}

int WorkerManager::getNumGasWorkers() 
{
	return workerData.getNumGasWorkers();
}

WorkerManager & WorkerManager::Instance() 
{
	static WorkerManager instance;
	return instance;
}

const WorkerData & WorkerManager::getData() const
{
	return workerData;
}