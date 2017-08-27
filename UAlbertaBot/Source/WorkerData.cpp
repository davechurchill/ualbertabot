#include "WorkerData.h"
#include "Micro.h"
#include <BWAPI/Game.h>
#include <BWAPI/Unitset.h>
#include <assert.h>

using namespace UAlbertaBot;

WorkerData::WorkerData(std::shared_ptr<AKBot::Logger> logger)
	: _logger(logger)
{
    for (auto & unit : BWAPI::Broodwar->getAllUnits())
    {
        if ((unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field))
        {
            _workersOnMineralPatch[unit] = 0;
        }
    }
}

void WorkerData::workerDestroyed(BWAPI::Unit unit)
{
    if (!unit) { 
		return;
	}

    clearPreviousJob(unit);
    _workers.erase(unit);
}

void WorkerData::registerWorker(BWAPI::Unit unit)
{
    if (!unit) { 
		return;
	}

    _workers.insert(unit);
    _workerJobMap[unit] = Default;
}

void WorkerData::addWorker(BWAPI::Unit worker, WorkerJob job, BWAPI::Unit jobUnit, int currentFrame)
{
	if (!worker || !jobUnit) { 
		return;
	}

	UAB_ASSERT(_workers.find(worker) == _workers.end(), "Worker was already in the set");

	_workers.insert(worker);
	setWorkerJob(worker, job, jobUnit, currentFrame);
}

void WorkerData::addWorker(BWAPI::Unit unit, enum WorkerJob job, BWAPI::UnitType jobUnitType)
{
	if (!unit) {
		return;
	}

	UAB_ASSERT(_workers.find(unit) == _workers.end(), "Worker was already in the set");
	_workers.insert(unit);
	setWorkerJob(unit, job, jobUnitType);
}

void WorkerData::registerResourceDepot(BWAPI::Unit unit)
{
	if (!unit) { return; }

	UAB_ASSERT(_depots.find(unit) == _depots.end(), "Depot was already in the set");
	_depots.insert(unit);
	_depotWorkerCount[unit] = 0;
}

void WorkerData::unregisterResourceDepot(BWAPI::Unit resourceDepot, int currentFrame)
{
    if (!resourceDepot) {
		return;
	}

    _depots.erase(resourceDepot);
    _depotWorkerCount.erase(resourceDepot);

    // re-balance workers in here
    for (auto & worker : _workers)
    {
        // if a worker was working at this depot
        if (_workerDepotMap[worker] == resourceDepot)
        {
            setWorkerJob(worker,Idle,nullptr, currentFrame);
        }
    }
}

void WorkerData::addToMineralPatch(BWAPI::Unit unit,int num)
{
    if (_workersOnMineralPatch.find(unit) == _workersOnMineralPatch.end())
    {
        _workersOnMineralPatch[unit] = num;
    }
    else
    {
        _workersOnMineralPatch[unit] = _workersOnMineralPatch[unit] + num;
    }
}

void WorkerData::setWorkerJob(BWAPI::Unit unit,enum WorkerJob job,BWAPI::Unit jobUnit, int currentFrame)
{
    if (!unit) { return; }

    clearPreviousJob(unit);
    _workerJobMap[unit] = job;

    if (job == Minerals)
    {
        // increase the number of workers assigned to this nexus
        _depotWorkerCount[jobUnit] += 1;

        // set the mineral the worker is working on
        _workerDepotMap[unit] = jobUnit;

        BWAPI::Unit mineralToMine = getMineralToMine(unit);
        _workerMineralMap[unit] = mineralToMine;
        addToMineralPatch(mineralToMine,1);

        // right click the mineral to start mining
        Micro::SmartRightClick(unit,mineralToMine, currentFrame);
    }
    else if (job == Gas)
    {
        // increase the count of workers assigned to this refinery
        _refineryWorkerCount[jobUnit] += 1;

        // set the refinery the worker is working on
        _workerRefineryMap[unit] = jobUnit;

        // right click the refinery to start harvesting
        Micro::SmartRightClick(unit,jobUnit, currentFrame);
    }
    else if (job == Repair)
    {
        // only SCVs can repair
        assert(unit->getType() == BWAPI::UnitTypes::Terran_SCV);

        // set the building the worker is to repair
        _workerRepairMap[unit] = jobUnit;

        // start repairing 
        if (!unit->isRepairing())
        {
            Micro::SmartRepair(unit,jobUnit, currentFrame);
        }
    }
    else if (job == Scout)
    {

    }
    else if (job == Build)
    {
        _logger->log("Setting worker job to build");
    }
}

void WorkerData::setWorkerJob(BWAPI::Unit unit,enum WorkerJob job,BWAPI::UnitType jobUnitType)
{
    if (!unit) { return; }

    clearPreviousJob(unit);
    _workerJobMap[unit] = job;

    if (job == Build)
    {
        _workerBuildingTypeMap[unit] = jobUnitType;
    }
}

void WorkerData::setWorkerJob(BWAPI::Unit unit,enum WorkerJob job,WorkerMoveData wmd)
{
    if (!unit) { return; }

    clearPreviousJob(unit);
    _workerJobMap[unit] = job;

    if (job == Move)
    {
        _workerMoveMap[unit] = wmd;
    }

    if (_workerJobMap[unit] != Move)
    {
        _logger->log("Something went horribly wrong");
    }
}


void WorkerData::clearPreviousJob(BWAPI::Unit unit)
{
    if (!unit) { return; }

    WorkerJob previousJob = getWorkerJob(unit);

    if (previousJob == Minerals)
    {
        _depotWorkerCount[_workerDepotMap[unit]] -= 1;

        _workerDepotMap.erase(unit);

        // remove a worker from this unit's assigned mineral patch
        addToMineralPatch(_workerMineralMap[unit],-1);

        // erase the association from the map
        _workerMineralMap.erase(unit);
    }
    else if (previousJob == Gas)
    {
        _refineryWorkerCount[_workerRefineryMap[unit]] -= 1;
        _workerRefineryMap.erase(unit);
    }
    else if (previousJob == Build)
    {
        _workerBuildingTypeMap.erase(unit);
    }
    else if (previousJob == Repair)
    {
        _workerRepairMap.erase(unit);
    }
    else if (previousJob == Move)
    {
        _workerMoveMap.erase(unit);
    }

    _workerJobMap.erase(unit);
}

int WorkerData::getNumWorkers() const
{
    return _workers.size();
}

int WorkerData::getNumMineralWorkers() const
{
    size_t num = 0;
    for (auto & unit : _workers)
    {
        if (_workerJobMap.at(unit) == WorkerData::Minerals)
        {
            num++;
        }
    }
    return num;
}

int WorkerData::getNumGasWorkers() const
{
    size_t num = 0;
    for (auto & unit : _workers)
    {
        if (_workerJobMap.at(unit) == WorkerData::Gas)
        {
            num++;
        }
    }
    return num;
}

int WorkerData::getNumIdleWorkers() const
{
    size_t num = 0;
    for (auto & unit : _workers)
    {
        if (_workerJobMap.at(unit) == WorkerData::Idle)
        {
            num++;
        }
    }
    return num;
}


enum WorkerData::WorkerJob WorkerData::getWorkerJob(BWAPI::Unit unit) const
{
    if (!unit || _workerJobMap.size() == 0) 
	{ 
		return Default; 
	}

    auto it = _workerJobMap.find(unit);
    if (it != _workerJobMap.end())
    {
        return it->second;
    }

    return Default;
}

bool WorkerData::depotIsFull(BWAPI::Unit depot)
{
    if (!depot) 
	{ 
		return false; 
	}

    int assignedWorkers = getNumAssignedWorkers(depot);
	if (assignedWorkers == 0 && depot->getType().isRefinery())
	{
		_refineryWorkerCount[depot] = 0;
	}

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

std::vector<BWAPI::Unit> WorkerData::getMineralPatchesNearDepot(BWAPI::Unit depot) const
{
    // if there are minerals near the depot, add them to the set
    std::vector<BWAPI::Unit> mineralsNearDepot;

    int radius = 300;

    for (auto & unit : BWAPI::Broodwar->getAllUnits())
    {
        if ((unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field) && unit->getDistance(depot) < radius)
        {
            mineralsNearDepot.push_back(unit);
        }
    }

    // if we didn't find any, use the whole map
    if (mineralsNearDepot.empty())
    {
        for (auto & unit : BWAPI::Broodwar->getAllUnits())
        {
            if ((unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field))
            {
                mineralsNearDepot.push_back(unit);
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

BWAPI::Unit WorkerData::getWorkerResource(BWAPI::Unit unit) const
{
    if (!unit) { return nullptr; }

    if (getWorkerJob(unit) == Minerals)
    {
        auto it = _workerMineralMap.find(unit);
        if (it != _workerMineralMap.end())
        {
            return it->second;
        }
    }
    else if (getWorkerJob(unit) == Gas)
    {
        auto it = _workerRefineryMap.find(unit);
        if (it != _workerRefineryMap.end())
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
        std::vector<BWAPI::Unit> mineralPatches = getMineralPatchesNearDepot(depot);

        for (auto & mineral : mineralPatches)
        {
            double dist = mineral->getDistance(depot);
            double numAssigned = _workersOnMineralPatch[mineral];

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

BWAPI::Unit WorkerData::getWorkerRepairUnit(BWAPI::Unit unit) const
{
    if (!unit) { return nullptr; }

    auto it = _workerRepairMap.find(unit);

    if (it != _workerRepairMap.end())
    {
        return it->second;
    }

    return nullptr;
}

BWAPI::Unit WorkerData::getWorkerDepot(BWAPI::Unit unit) const
{
    if (!unit) { return nullptr; }

    auto it = _workerDepotMap.find(unit);

    if (it != _workerDepotMap.end())
    {
        return it->second;
    }

    return nullptr;
}

BWAPI::UnitType	WorkerData::getWorkerBuildingType(BWAPI::Unit unit) const
{
    if (!unit) { return BWAPI::UnitTypes::None; }

    auto it = _workerBuildingTypeMap.find(unit);

    if (it != _workerBuildingTypeMap.end())
    {
        return it->second;
    }

    return BWAPI::UnitTypes::None;
}

WorkerMoveData WorkerData::getWorkerMoveData(BWAPI::Unit unit) const
{
    auto it = _workerMoveMap.find(unit);

    UAB_ASSERT(it != _workerMoveMap.end(),"Worker not found");

    return (it->second);
}

int WorkerData::getNumAssignedWorkers(BWAPI::Unit unit) const
{
    if (!unit)
	{ 
		return 0;
	}

    if (unit->getType().isResourceDepot())
    {
        auto it = _depotWorkerCount.find(unit);

        // if there is an entry, return it
        if (it != _depotWorkerCount.end())
        {
            return it->second;
        }
    }
    else if (unit->getType().isRefinery())
    {
        auto it = _refineryWorkerCount.find(unit);

        // if there is an entry, return it
        if (it != _refineryWorkerCount.end())
        {
            return it->second;
        }
    }

    // when all else fails, return 0
    return 0;
}

char WorkerData::getJobCode(BWAPI::Unit unit) const
{
    if (!unit) { return 'N'; }

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

const std::set<BWAPI::Unit> & WorkerData::getWorkers() const
{
    return _workers;
}

const std::set<BWAPI::Unit> & WorkerData::getDepots() const
{
	return _depots;
}

const std::map<BWAPI::Unit, int>& WorkerData::getWorkersOnMineralPatch() const
{
	return _workersOnMineralPatch;
}