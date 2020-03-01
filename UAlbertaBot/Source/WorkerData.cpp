#include "WorkerData.h"
#include "Micro.h"

using namespace UAlbertaBot;

WorkerData::WorkerData()
{
    for (auto & unit : BWAPI::Broodwar->getAllUnits())
    {
        if ((unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field))
        {
            m_workersOnMineralPatch[unit] = 0;
        }
    }
}

void WorkerData::workerDestroyed(BWAPI::Unit unit)
{
    if (!unit) { return; }

    clearPreviousJob(unit);
    m_workers.erase(unit);
}

void WorkerData::addWorker(BWAPI::Unit unit)
{
    if (!unit) { return; }

    m_workers.insert(unit);
    m_workerJobMap[unit] = Default;
}

void WorkerData::addWorker(BWAPI::Unit unit, WorkerJob job, BWAPI::Unit jobUnit)
{
    if (!unit || !jobUnit) { return; }

    assert(m_workers.find(unit) == m_workers.end());

    m_workers.insert(unit);
    setWorkerJob(unit, job, jobUnit);
}

void WorkerData::addWorker(BWAPI::Unit unit, enum WorkerJob job, BWAPI::UnitType jobUnitType)
{
    if (!unit) { return; }

    assert(m_workers.find(unit) == m_workers.end());
    m_workers.insert(unit);
    setWorkerJob(unit, job, jobUnitType);
}

void WorkerData::addDepot(BWAPI::Unit unit)
{
    if (!unit) { return; }

    assert(m_depots.find(unit) == m_depots.end());
    m_depots.insert(unit);
    m_depotWorkerCount[unit] = 0;
}

void WorkerData::removeDepot(BWAPI::Unit unit)
{
    if (!unit) { return; }

    m_depots.erase(unit);
    m_depotWorkerCount.erase(unit);

    // re-balance workers in here
    for (auto & worker : m_workers)
    {
        // if a worker was working at this depot
        if (m_workerDepotMap[worker] == unit)
        {
            setWorkerJob(worker, Idle, nullptr);
        }
    }
}

void WorkerData::addToMineralPatch(BWAPI::Unit unit, int num)
{
    if (m_workersOnMineralPatch.find(unit) == m_workersOnMineralPatch.end())
    {
        m_workersOnMineralPatch[unit] = num;
    }
    else
    {
        m_workersOnMineralPatch[unit] = m_workersOnMineralPatch[unit] + num;
    }
}

void WorkerData::setWorkerJob(BWAPI::Unit unit, enum WorkerJob job, BWAPI::Unit jobUnit)
{
    if (!unit) { return; }

    clearPreviousJob(unit);
    m_workerJobMap[unit] = job;

    if (job == Minerals)
    {
        // increase the number of workers assigned to this nexus
        m_depotWorkerCount[jobUnit] += 1;

        // set the mineral the worker is working on
        m_workerDepotMap[unit] = jobUnit;

        BWAPI::Unit mineralToMine = getMineralToMine(unit);
        m_workerMineralAssignment[unit] = mineralToMine;
        addToMineralPatch(mineralToMine, 1);

        // right click the mineral to start mining
        Micro::SmartRightClick(unit, mineralToMine);
    }
    else if (job == Gas)
    {
        // increase the count of workers assigned to this refinery
        m_refineryWorkerCount[jobUnit] += 1;

        // set the refinery the worker is working on
        m_workerRefineryMap[unit] = jobUnit;

        // right click the refinery to start harvesting
        Micro::SmartRightClick(unit, jobUnit);
    }
    else if (job == Repair)
    {
        // only SCVs can repair
        assert(unit->getType() == BWAPI::UnitTypes::Terran_SCV);

        // set the building the worker is to repair
        m_workerRepairMap[unit] = jobUnit;

        // start repairing 
        if (!unit->isRepairing())
        {
            Micro::SmartRepair(unit, jobUnit);
        }
    }
    else if (job == Scout)
    {

    }
    else if (job == Build)
    {
        BWAPI::Broodwar->printf("Setting worker job to build");
    }
}

void WorkerData::setWorkerJob(BWAPI::Unit unit, enum WorkerJob job, BWAPI::UnitType jobUnitType)
{
    if (!unit) { return; }

    clearPreviousJob(unit);
    m_workerJobMap[unit] = job;

    if (job == Build)
    {
        m_workerBuildingTypeMap[unit] = jobUnitType;
    }
}

void WorkerData::setWorkerJob(BWAPI::Unit unit, enum WorkerJob job, WorkerMoveData wmd)
{
    if (!unit) { return; }

    clearPreviousJob(unit);
    m_workerJobMap[unit] = job;

    if (job == Move)
    {
        m_workerMoveMap[unit] = wmd;
    }

    if (m_workerJobMap[unit] != Move)
    {
        BWAPI::Broodwar->printf("Something went horribly wrong");
    }
}


void WorkerData::clearPreviousJob(BWAPI::Unit unit)
{
    if (!unit) { return; }

    WorkerJob previousJob = getWorkerJob(unit);

    if (previousJob == Minerals)
    {
        m_depotWorkerCount[m_workerDepotMap[unit]] -= 1;

        m_workerDepotMap.erase(unit);

        // remove a worker from this unit's assigned mineral patch
        addToMineralPatch(m_workerMineralAssignment[unit], -1);

        // erase the association from the map
        m_workerMineralAssignment.erase(unit);
    }
    else if (previousJob == Gas)
    {
        m_refineryWorkerCount[m_workerRefineryMap[unit]] -= 1;
        m_workerRefineryMap.erase(unit);
    }
    else if (previousJob == Build)
    {
        m_workerBuildingTypeMap.erase(unit);
    }
    else if (previousJob == Repair)
    {
        m_workerRepairMap.erase(unit);
    }
    else if (previousJob == Move)
    {
        m_workerMoveMap.erase(unit);
    }

    m_workerJobMap.erase(unit);
}

int WorkerData::getNumWorkers() const
{
    return m_workers.size();
}

int WorkerData::getNumMineralWorkers() const
{
    size_t num = 0;
    for (auto & unit : m_workers)
    {
        if (m_workerJobMap.at(unit) == WorkerData::Minerals)
        {
            num++;
        }
    }
    return num;
}

int WorkerData::getNumGasWorkers() const
{
    size_t num = 0;
    for (auto & unit : m_workers)
    {
        if (m_workerJobMap.at(unit) == WorkerData::Gas)
        {
            num++;
        }
    }
    return num;
}

int WorkerData::getNumIdleWorkers() const
{
    size_t num = 0;
    for (auto & unit : m_workers)
    {
        if (m_workerJobMap.at(unit) == WorkerData::Idle)
        {
            num++;
        }
    }
    return num;
}


enum WorkerData::WorkerJob WorkerData::getWorkerJob(BWAPI::Unit unit)
{
    if (!unit) { return Default; }

    std::map<BWAPI::Unit, enum WorkerJob>::iterator it = m_workerJobMap.find(unit);

    if (it != m_workerJobMap.end())
    {
        return it->second;
    }

    return Default;
}

bool WorkerData::depotIsFull(BWAPI::Unit depot)
{
    if (!depot) { return false; }

    int assignedWorkers = getNumAssignedWorkers(depot);
    int mineralsNearDepot = getMineralsNearDepot(depot);

    if (assignedWorkers >= mineralsNearDepot * 3)
    {
        return true;
    }
    else
    {
        return false;
    }
}

BWAPI::Unitset WorkerData::getMineralPatchesNearDepot(BWAPI::Unit depot)
{
    // if there are minerals near the depot, add them to the set
    BWAPI::Unitset mineralsNearDepot;

    int radius = 300;

    for (auto & unit : BWAPI::Broodwar->getAllUnits())
    {
        if ((unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field) && unit->getDistance(depot) < radius)
        {
            mineralsNearDepot.insert(unit);
        }
    }

    // if we didn't find any, use the whole map
    if (mineralsNearDepot.empty())
    {
        for (auto & unit : BWAPI::Broodwar->getAllUnits())
        {
            if ((unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field))
            {
                mineralsNearDepot.insert(unit);
            }
        }
    }

    return mineralsNearDepot;
}

int WorkerData::getMineralsNearDepot(BWAPI::Unit depot)
{
    if (!depot) { return 0; }

    int mineralsNearDepot = 0;

    for (auto & unit : BWAPI::Broodwar->getAllUnits())
    {
        if ((unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field) && unit->getDistance(depot) < 200)
        {
            mineralsNearDepot++;
        }
    }

    return mineralsNearDepot;
}

BWAPI::Unit WorkerData::getWorkerResource(BWAPI::Unit unit)
{
    if (!unit) { return nullptr; }

    // create the iterator
    std::map<BWAPI::Unit, BWAPI::Unit>::iterator it;

    // if the worker is mining, set the iterator to the mineral map
    if (getWorkerJob(unit) == Minerals)
    {
        it = m_workerMineralMap.find(unit);
        if (it != m_workerMineralMap.end())
        {
            return it->second;
        }
    }
    else if (getWorkerJob(unit) == Gas)
    {
        it = m_workerRefineryMap.find(unit);
        if (it != m_workerRefineryMap.end())
        {
            return it->second;
        }
    }

    return nullptr;
}


BWAPI::Unit WorkerData::getMineralToMine(BWAPI::Unit worker)
{
    if (!worker) { return nullptr; }


    // get the depot associated with this unit
    BWAPI::Unit depot = getWorkerDepot(worker);
    BWAPI::Unit bestMineral = nullptr;
    double bestDist = 100000;
    double bestNumAssigned = 10000;

    if (depot)
    {
        BWAPI::Unitset mineralPatches = getMineralPatchesNearDepot(depot);

        for (auto & mineral : mineralPatches)
        {
            double dist = mineral->getDistance(depot);
            double numAssigned = m_workersOnMineralPatch[mineral];

            if (numAssigned < bestNumAssigned)
            {
                bestMineral = mineral;
                bestDist = dist;
                bestNumAssigned = numAssigned;
            }
            else if (numAssigned == bestNumAssigned)
            {
                if (dist < bestDist)
                {
                    bestMineral = mineral;
                    bestDist = dist;
                    bestNumAssigned = numAssigned;
                }
            }

        }
    }

    return bestMineral;
}
/*
BWAPI::Unit WorkerData::getMineralToMine(BWAPI::Unit worker)
{
    if (!worker) { return nullptr; }

    // get the depot associated with this unit
    BWAPI::Unit depot = getWorkerDepot(worker);
    BWAPI::Unit mineral = nullptr;
    double closestDist = 10000;

    if (depot)
    {
        BOOST_FOREACH (BWAPI::Unit unit, BWAPI::Broodwar->getAllUnits())
        {
            if (unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field && unit->getResources() > 0)
            {
                double dist = unit->getDistance(depot);

                if (!mineral || dist < closestDist)
                {
                    mineral = unit;
                    closestDist = dist;
                }
            }
        }
    }

    return mineral;
}*/

BWAPI::Unit WorkerData::getWorkerRepairUnit(BWAPI::Unit unit)
{
    if (!unit) { return nullptr; }

    std::map<BWAPI::Unit, BWAPI::Unit>::iterator it = m_workerRepairMap.find(unit);

    if (it != m_workerRepairMap.end())
    {
        return it->second;
    }

    return nullptr;
}

BWAPI::Unit WorkerData::getWorkerDepot(BWAPI::Unit unit)
{
    if (!unit) { return nullptr; }

    std::map<BWAPI::Unit, BWAPI::Unit>::iterator it = m_workerDepotMap.find(unit);

    if (it != m_workerDepotMap.end())
    {
        return it->second;
    }

    return nullptr;
}

BWAPI::UnitType	WorkerData::getWorkerBuildingType(BWAPI::Unit unit)
{
    if (!unit) { return BWAPI::UnitTypes::None; }

    std::map<BWAPI::Unit, BWAPI::UnitType>::iterator it = m_workerBuildingTypeMap.find(unit);

    if (it != m_workerBuildingTypeMap.end())
    {
        return it->second;
    }

    return BWAPI::UnitTypes::None;
}

WorkerMoveData WorkerData::getWorkerMoveData(BWAPI::Unit unit)
{
    std::map<BWAPI::Unit, WorkerMoveData>::iterator it = m_workerMoveMap.find(unit);

    assert(it != m_workerMoveMap.end());

    return (it->second);
}

int WorkerData::getNumAssignedWorkers(BWAPI::Unit unit)
{
    if (!unit) { return 0; }

    std::map<BWAPI::Unit, int>::iterator it;

    // if the worker is mining, set the iterator to the mineral map
    if (unit->getType().isResourceDepot())
    {
        it = m_depotWorkerCount.find(unit);

        // if there is an entry, return it
        if (it != m_depotWorkerCount.end())
        {
            return it->second;
        }
    }
    else if (unit->getType().isRefinery())
    {
        it = m_refineryWorkerCount.find(unit);

        // if there is an entry, return it
        if (it != m_refineryWorkerCount.end())
        {
            return it->second;
        }
        // otherwise, we are only calling this on completed refineries, so set it
        else
        {
            m_refineryWorkerCount[unit] = 0;
        }
    }

    // when all else fails, return 0
    return 0;
}

char WorkerData::getJobCode(BWAPI::Unit unit)
{
    if (!unit) { return 'X'; }

    WorkerData::WorkerJob j = getWorkerJob(unit);

    if (j == WorkerData::Build) return 'B';
    if (j == WorkerData::Combat) return 'C';
    if (j == WorkerData::Default) return 'D';
    if (j == WorkerData::Gas) return 'G';
    if (j == WorkerData::Idle) return 'I';
    if (j == WorkerData::Minerals) return 'M';
    if (j == WorkerData::Repair) return 'R';
    if (j == WorkerData::Move) return 'O';
    if (j == WorkerData::Scout) return 'S';
    return 'X';
}

void WorkerData::drawDepotDebugInfo()
{
    for (auto & depot : m_depots)
    {
        int x = depot->getPosition().x - 64;
        int y = depot->getPosition().y - 32;

        if (Config::Debug::DrawWorkerInfo) BWAPI::Broodwar->drawBoxMap(x-2, y-1, x+75, y+14, BWAPI::Colors::Black, true);
        if (Config::Debug::DrawWorkerInfo) BWAPI::Broodwar->drawTextMap(x, y, "\x04 Workers: %d", getNumAssignedWorkers(depot));

        BWAPI::Unitset minerals = getMineralPatchesNearDepot(depot);

        for (auto & mineral : minerals)
        {
            int x = mineral->getPosition().x;
            int y = mineral->getPosition().y;

            if (m_workersOnMineralPatch.find(mineral) != m_workersOnMineralPatch.end())
            {
                //if (Config::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawBoxMap(x-2, y-1, x+75, y+14, BWAPI::Colors::Black, true);
                //if (Config::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextMap(x, y, "\x04 Workers: %d", workersOnMineralPatch[mineral]);
            }
        }
    }
}

const BWAPI::Unitset & WorkerData::getWorkers() const
{
    return m_workers;
}