#include "Common.h"
#include "WorkerManager.h"
#include "Micro.h"
#include "BaseLocationManager.h"
#include "Global.h"
#include "BuildingData.h"

using namespace UAlbertaBot;

WorkerManager::WorkerManager()
{

}

void WorkerManager::onFrame()
{
    PROFILE_FUNCTION();

    updateWorkerStatus();
    handleGasWorkers();
    handleIdleWorkers();
    handleMoveWorkers();
    handleCombatWorkers();

    drawResourceDebugInfo();
    drawWorkerInformation(450, 20);

    m_workerData.drawDepotDebugInfo();

    handleRepairWorkers();
}

void WorkerManager::updateWorkerStatus()
{
    PROFILE_FUNCTION();

    // for each of our Workers
    for (auto & worker : m_workerData.getWorkers())
    {
        if (!worker->isCompleted())
        {
            continue;
        }

        // if it's idle
        if (worker->isIdle() &&
            (m_workerData.getWorkerJob(worker) != WorkerData::Build) &&
            (m_workerData.getWorkerJob(worker) != WorkerData::Move) &&
            (m_workerData.getWorkerJob(worker) != WorkerData::Scout))
        {
            m_workerData.setWorkerJob(worker, WorkerData::Idle, nullptr);
        }

        // if its job is gas
        if (m_workerData.getWorkerJob(worker) == WorkerData::Gas)
        {
            const BWAPI::Unit refinery = m_workerData.getWorkerResource(worker);

            // if the refinery doesn't exist anymore
            if (!refinery || !refinery->exists() ||	refinery->getHitPoints() <= 0)
            {
                setMineralWorker(worker);
            }
        }
    }
}

void WorkerManager::setRepairWorker(BWAPI::Unit worker, BWAPI::Unit unitToRepair)
{
    m_workerData.setWorkerJob(worker, WorkerData::Repair, unitToRepair);
}

void WorkerManager::stopRepairing(BWAPI::Unit worker)
{
    m_workerData.setWorkerJob(worker, WorkerData::Idle, nullptr);
}

void WorkerManager::handleGasWorkers()
{
    // for each unit we have
    for (auto & unit : BWAPI::Broodwar->self()->getUnits())
    {
        // if that unit is a refinery
        if (unit->getType().isRefinery() && unit->isCompleted() && !isGasStealRefinery(unit))
        {
            // get the number of workers currently assigned to it
            const int numAssigned = m_workerData.getNumAssignedWorkers(unit);

            // if it's less than we want it to be, fill 'er up
            for (int i=0; i<(Config::Macro::WorkersPerRefinery-numAssigned); ++i)
            {
                BWAPI::Unit gasWorker = getGasWorker(unit);
                if (gasWorker)
                {
                    m_workerData.setWorkerJob(gasWorker, WorkerData::Gas, unit);
                }
            }
        }
    }

}

bool WorkerManager::isGasStealRefinery(BWAPI::Unit unit)
{
    auto * enemyBaseLocation = Global::Bases().getPlayerStartingBaseLocation(BWAPI::Broodwar->enemy());
    if (!enemyBaseLocation)
    {
        return false;
    }

    if (enemyBaseLocation->getGeysers().empty())
    {
        return false;
    }

    for (auto & u : enemyBaseLocation->getGeysers())
    {
        if (unit->getTilePosition() == u->getTilePosition())
        {
            return true;
        }
    }

    return false;
}

void WorkerManager::handleIdleWorkers()
{
    // for each of our workers
    for (auto & worker : m_workerData.getWorkers())
    {
        UAB_ASSERT(worker != nullptr, "Worker was null");

        // if it is idle
        if (m_workerData.getWorkerJob(worker) == WorkerData::Idle)
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

    for (auto & unit : BWAPI::Broodwar->self()->getUnits())
    {
        if (unit->getType().isBuilding() && (unit->getHitPoints() < unit->getType().maxHitPoints()))
        {
            const BWAPI::Unit repairWorker = getClosestMineralWorkerTo(unit);
            setRepairWorker(repairWorker, unit);
            break;
        }
    }
}

// bad micro for combat workers
void WorkerManager::handleCombatWorkers()
{
    for (auto & worker : m_workerData.getWorkers())
    {
        UAB_ASSERT(worker != nullptr, "Worker was null");

        if (m_workerData.getWorkerJob(worker) == WorkerData::Combat)
        {
            BWAPI::Broodwar->drawCircleMap(worker->getPosition().x, worker->getPosition().y, 4, BWAPI::Colors::Yellow, true);
            const BWAPI::Unit target = getClosestEnemyUnit(worker);

            if (target)
            {
                Micro::SmartAttackUnit(worker, target);
            }
        }
    }
}

BWAPI::Unit WorkerManager::getClosestEnemyUnit(BWAPI::Unit worker)
{
    UAB_ASSERT(worker != nullptr, "Worker was null");

    BWAPI::Unit closestUnit = nullptr;
    double closestDist = 10000;

    for (auto & unit : BWAPI::Broodwar->enemy()->getUnits())
    {
        const double dist = unit->getDistance(worker);

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
    for (auto & worker : m_workerData.getWorkers())
    {
        UAB_ASSERT(worker != nullptr, "Worker was null");

        if (m_workerData.getWorkerJob(worker) == WorkerData::Combat)
        {
            setMineralWorker(worker);
        }
    }
}

BWAPI::Unit WorkerManager::getClosestMineralWorkerTo(BWAPI::Unit enemyUnit)
{
    UAB_ASSERT(enemyUnit != nullptr, "enemyUnit was null");

    BWAPI::Unit closestMineralWorker = nullptr;
    double closestDist = 100000;

    if (m_previousClosestWorker)
    {
        if (m_previousClosestWorker->getHitPoints() > 0)
        {
            return m_previousClosestWorker;
        }
    }

    // for each of our workers
    for (auto & worker : m_workerData.getWorkers())
    {
        UAB_ASSERT(worker != nullptr, "Worker was null");
        if (!worker)
        {
            continue;
        }
        // if it is a move worker
        if (m_workerData.getWorkerJob(worker) == WorkerData::Minerals)
        {
            double dist = worker->getDistance(enemyUnit);

            if (!closestMineralWorker || dist < closestDist)
            {
                closestMineralWorker = worker;
                closestDist = dist;
            }
        }
    }

    m_previousClosestWorker = closestMineralWorker;
    return closestMineralWorker;
}

BWAPI::Unit WorkerManager::getWorkerScout()
{
    // for each of our workers
    for (auto & worker : m_workerData.getWorkers())
    {
        UAB_ASSERT(worker != nullptr, "Worker was null");
        if (!worker)
        {
            continue;
        }
        // if it is a move worker
        if (m_workerData.getWorkerJob(worker) == WorkerData::Scout)
        {
            return worker;
        }
    }

    return nullptr;
}

void WorkerManager::handleMoveWorkers()
{
    // for each of our workers
    for (auto & worker : m_workerData.getWorkers())
    {
        UAB_ASSERT(worker != nullptr, "Worker was null");

        // if it is a move worker
        if (m_workerData.getWorkerJob(worker) == WorkerData::Move)
        {
            const WorkerMoveData data = m_workerData.getWorkerMoveData(worker);

            Micro::SmartMove(worker, data.position);
        }
    }
}

// set a worker to mine minerals
void WorkerManager::setMineralWorker(BWAPI::Unit unit)
{
    UAB_ASSERT(unit != nullptr, "Unit was null");

    // check if there is a mineral available to send the worker to
    BWAPI::Unit depot = getClosestDepot(unit);

    // if there is a valid mineral
    if (depot)
    {
        // update workerData with the new job
        m_workerData.setWorkerJob(unit, WorkerData::Minerals, depot);
    }
    else
    {
        // BWAPI::Broodwar->printf("No valid depot for mineral worker");
    }
}

BWAPI::Unit WorkerManager::getClosestDepot(BWAPI::Unit worker)
{
    UAB_ASSERT(worker != nullptr, "Worker was null");

    BWAPI::Unit closestDepot = nullptr;
    double closestDistance = 0;

    for (auto & unit : BWAPI::Broodwar->self()->getUnits())
    {
        UAB_ASSERT(unit != nullptr, "Unit was null");

        if (unit->getType().isResourceDepot() && (unit->isCompleted() || unit->getType() == BWAPI::UnitTypes::Zerg_Lair) && !m_workerData.depotIsFull(unit))
        {
            const double distance = unit->getDistance(worker);
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
void WorkerManager::finishedWithWorker(BWAPI::Unit unit)
{
    UAB_ASSERT(unit != nullptr, "Unit was null");

    //BWAPI::Broodwar->printf("BuildingManager finished with worker %d", unit->getID());
    if (m_workerData.getWorkerJob(unit) != WorkerData::Scout)
    {
        m_workerData.setWorkerJob(unit, WorkerData::Idle, nullptr);
    }
}

BWAPI::Unit WorkerManager::getGasWorker(BWAPI::Unit refinery)
{
    UAB_ASSERT(refinery != nullptr, "Refinery was null");

    BWAPI::Unit closestWorker = nullptr;
    double closestDistance = 0;

    for (auto & unit : m_workerData.getWorkers())
    {
        UAB_ASSERT(unit != nullptr, "Unit was null");

        if (m_workerData.getWorkerJob(unit) == WorkerData::Minerals)
        {
            const double distance = unit->getDistance(refinery);
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

    m_workerData.setWorkerJob(worker, WorkerData::Build, b.type);
}

// gets a builder for BuildingManager to use
// if setJobAsBuilder is true (default), it will be flagged as a builder unit
// set 'setJobAsBuilder' to false if we just want to see which worker will build a building
BWAPI::Unit WorkerManager::getBuilder(Building & b, bool setJobAsBuilder)
{
    // variables to hold the closest worker of each type to the building
    BWAPI::Unit closestMovingWorker = nullptr;
    BWAPI::Unit closestMiningWorker = nullptr;
    double closestMovingWorkerDistance = 0;
    double closestMiningWorkerDistance = 0;

    // look through each worker that had moved there first
    for (auto & unit : m_workerData.getWorkers())
    {
        UAB_ASSERT(unit != nullptr, "Unit was null");

        // gas steal building uses scout worker
        if (b.isGasSteal && (m_workerData.getWorkerJob(unit) == WorkerData::Scout))
        {
            if (setJobAsBuilder)
            {
                m_workerData.setWorkerJob(unit, WorkerData::Build, b.type);
            }
            return unit;
        }

        // mining worker check
        if (unit->isCompleted() && (m_workerData.getWorkerJob(unit) == WorkerData::Minerals))
        {
            // if it is a new closest distance, set the pointer
            const double distance = unit->getDistance(BWAPI::Position(b.finalPosition));
            if (!closestMiningWorker || distance < closestMiningWorkerDistance)
            {
                closestMiningWorker = unit;
                closestMiningWorkerDistance = distance;
            }
        }

        // moving worker check
        if (unit->isCompleted() && (m_workerData.getWorkerJob(unit) == WorkerData::Move))
        {
            // if it is a new closest distance, set the pointer
            const double distance = unit->getDistance(BWAPI::Position(b.finalPosition));
            if (!closestMovingWorker || distance < closestMovingWorkerDistance)
            {
                closestMovingWorker = unit;
                closestMovingWorkerDistance = distance;
            }
        }
    }

    // if we found a moving worker, use it, otherwise using a mining worker
    BWAPI::Unit chosenWorker = closestMovingWorker ? closestMovingWorker : closestMiningWorker;

    // if the worker exists (one may not have been found in rare cases)
    if (chosenWorker && setJobAsBuilder)
    {
        m_workerData.setWorkerJob(chosenWorker, WorkerData::Build, b.type);
    }

    // return the worker
    return chosenWorker;
}

// sets a worker as a scout
void WorkerManager::setScoutWorker(BWAPI::Unit worker)
{
    UAB_ASSERT(worker != nullptr, "Worker was null");

    m_workerData.setWorkerJob(worker, WorkerData::Scout, nullptr);
}

// gets a worker which will move to a current location
BWAPI::Unit WorkerManager::getMoveWorker(BWAPI::Position p)
{
    // set up the pointer
    BWAPI::Unit closestWorker = nullptr;
    double closestDistance = 0;

    // for each worker we currently have
    for (auto & unit : m_workerData.getWorkers())
    {
        UAB_ASSERT(unit != nullptr, "Unit was null");

        // only consider it if it's a mineral worker
        if (unit->isCompleted() && m_workerData.getWorkerJob(unit) == WorkerData::Minerals)
        {
            // if it is a new closest distance, set the pointer
            const double distance = unit->getDistance(p);
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
    for (auto & unit : m_workerData.getWorkers())
    {
        UAB_ASSERT(unit != nullptr, "Unit was null");

        // only consider it if it's a mineral worker
        if (unit->isCompleted() && m_workerData.getWorkerJob(unit) == WorkerData::Minerals)
        {
            // if it is a new closest distance, set the pointer
            const double distance = unit->getDistance(p);
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
        m_workerData.setWorkerJob(closestWorker, WorkerData::Move, WorkerMoveData(mineralsNeeded, gasNeeded, p));
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
    const double speed = BWAPI::Broodwar->self()->getRace().getWorker().topSpeed();

    UAB_ASSERT(speed > 0, "Speed is negative");

    // how many frames it will take us to move to the building location
    // add a second to account for worker getting stuck. better early than late
    const double framesToMove = (distance / speed) + 50;

    // magic numbers to predict income rates
    const double mineralRate = getNumMineralWorkers() * 0.045;
    const double gasRate     = getNumGasWorkers() * 0.07;

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

void WorkerManager::setCombatWorker(BWAPI::Unit worker)
{
    UAB_ASSERT(worker != nullptr, "Worker was null");

    m_workerData.setWorkerJob(worker, WorkerData::Combat, nullptr);
}

void WorkerManager::onUnitMorph(BWAPI::Unit unit)
{
    UAB_ASSERT(unit != nullptr, "Unit was null");

    // if something morphs into a worker, add it
    if (unit->getType().isWorker() && unit->getPlayer() == BWAPI::Broodwar->self() && unit->getHitPoints() >= 0)
    {
        m_workerData.addWorker(unit);
    }

    // if something morphs into a building, it was a worker?
    if (unit->getType().isBuilding() && unit->getPlayer() == BWAPI::Broodwar->self() && unit->getPlayer()->getRace() == BWAPI::Races::Zerg)
    {
        //BWAPI::Broodwar->printf("A Drone started building");
        m_workerData.workerDestroyed(unit);
    }
}

void WorkerManager::onUnitShow(BWAPI::Unit unit)
{
    UAB_ASSERT(unit != nullptr, "Unit was null");

    // add the depot if it exists
    if (unit->getType().isResourceDepot() && unit->getPlayer() == BWAPI::Broodwar->self())
    {
        m_workerData.addDepot(unit);
    }

    // if something morphs into a worker, add it
    if (unit->getType().isWorker() && unit->getPlayer() == BWAPI::Broodwar->self() && unit->getHitPoints() >= 0)
    {
        //BWAPI::Broodwar->printf("A worker was shown %d", unit->getID());
        m_workerData.addWorker(unit);
    }
}


void WorkerManager::rebalanceWorkers()
{
    // for each worker
    for (auto & worker : m_workerData.getWorkers())
    {
        UAB_ASSERT(worker != nullptr, "Worker was null");

        if (!m_workerData.getWorkerJob(worker) == WorkerData::Minerals)
        {
            continue;
        }

        BWAPI::Unit depot = m_workerData.getWorkerDepot(worker);

        if (depot && m_workerData.depotIsFull(depot))
        {
            m_workerData.setWorkerJob(worker, WorkerData::Idle, nullptr);
        }
        else if (!depot)
        {
            m_workerData.setWorkerJob(worker, WorkerData::Idle, nullptr);
        }
    }
}

void WorkerManager::onUnitDestroy(BWAPI::Unit unit)
{
    UAB_ASSERT(unit != nullptr, "Unit was null");

    if (unit->getType().isResourceDepot() && unit->getPlayer() == BWAPI::Broodwar->self())
    {
        m_workerData.removeDepot(unit);
    }

    if (unit->getType().isWorker() && unit->getPlayer() == BWAPI::Broodwar->self())
    {
        m_workerData.workerDestroyed(unit);
    }

    if (unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field)
    {
        rebalanceWorkers();
    }
}

void WorkerManager::drawResourceDebugInfo()
{
    if (!Config::Debug::DrawResourceInfo)
    {
        return;
    }

    for (auto & worker : m_workerData.getWorkers())
    {
        UAB_ASSERT(worker != nullptr, "Worker was null");

        const char job = m_workerData.getJobCode(worker);

        const BWAPI::Position pos = worker->getTargetPosition();

        BWAPI::Broodwar->drawTextMap(worker->getPosition().x, worker->getPosition().y - 5, "\x07%c", job);

        BWAPI::Broodwar->drawLineMap(worker->getPosition().x, worker->getPosition().y, pos.x, pos.y, BWAPI::Colors::Cyan);

        const BWAPI::Unit depot = m_workerData.getWorkerDepot(worker);
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

    BWAPI::Broodwar->drawTextScreen(x, y, "\x04 Workers %d", m_workerData.getNumMineralWorkers());
    BWAPI::Broodwar->drawTextScreen(x, y+20, "\x04 UnitID");
    BWAPI::Broodwar->drawTextScreen(x+50, y+20, "\x04 State");

    int yspace = 0;

    for (auto & unit : m_workerData.getWorkers())
    {
        UAB_ASSERT(unit != nullptr, "Worker was null");

        BWAPI::Broodwar->drawTextScreen(x, y+40+((yspace)*10), "\x03 %d", unit->getID());
        BWAPI::Broodwar->drawTextScreen(x+50, y+40+((yspace++)*10), "\x03 %c", m_workerData.getJobCode(unit));
    }
}

bool WorkerManager::isFree(BWAPI::Unit worker)
{
    UAB_ASSERT(worker != nullptr, "Worker was null");

    return m_workerData.getWorkerJob(worker) == WorkerData::Minerals || m_workerData.getWorkerJob(worker) == WorkerData::Idle;
}

bool WorkerManager::isWorkerScout(BWAPI::Unit worker)
{
    UAB_ASSERT(worker != nullptr, "Worker was null");

    return (m_workerData.getWorkerJob(worker) == WorkerData::Scout);
}

bool WorkerManager::isBuilder(BWAPI::Unit worker)
{
    UAB_ASSERT(worker != nullptr, "Worker was null");

    return (m_workerData.getWorkerJob(worker) == WorkerData::Build);
}

int WorkerManager::getNumMineralWorkers()
{
    return m_workerData.getNumMineralWorkers();
}

int WorkerManager::getNumIdleWorkers()
{
    return m_workerData.getNumIdleWorkers();
}

int WorkerManager::getNumGasWorkers()
{
    return m_workerData.getNumGasWorkers();
}
