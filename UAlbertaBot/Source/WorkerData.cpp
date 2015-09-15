#include "WorkerData.h"

using namespace UAlbertaBot;

WorkerData::WorkerData() 
{
     for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->getAllUnits())
	{
		if ((unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field))
		{
            workersOnMineralPatch[unit] = 0;
		}
	}
}

void WorkerData::workerDestroyed(BWAPI::UnitInterface* unit)
{
	if (!unit) { return; }

	clearPreviousJob(unit);
	workers.erase(unit);
}

void WorkerData::addWorker(BWAPI::UnitInterface* unit)
{
	if (!unit) { return; }

	workers.insert(unit);
	workerJobMap[unit] = Default;
}

void WorkerData::addWorker(BWAPI::UnitInterface* unit, WorkerJob job, BWAPI::UnitInterface* jobUnit)
{
	if (!unit || !jobUnit) { return; }

	assert(workers.find(unit) == workers.end());

	workers.insert(unit);
	setWorkerJob(unit, job, jobUnit);
}

void WorkerData::addWorker(BWAPI::UnitInterface* unit, enum WorkerJob job, BWAPI::UnitType jobUnitType)
{
	if (!unit) { return; }

	assert(workers.find(unit) == workers.end());
	workers.insert(unit);
	setWorkerJob(unit, job, jobUnitType);
}

void WorkerData::addDepot(BWAPI::UnitInterface* unit)
{
	if (!unit) { return; }

	assert(depots.find(unit) == depots.end());
	depots.insert(unit);
	depotWorkerCount[unit] = 0;
}

void WorkerData::removeDepot(BWAPI::UnitInterface* unit)
{	
	if (!unit) { return; }

	depots.erase(unit);
	depotWorkerCount.erase(unit);

	// re-balance workers in here
	for (BWAPI::UnitInterface* worker : workers)
	{
		// if a worker was working at this depot
		if (workerDepotMap[worker] == unit)
		{
			setWorkerJob(worker, Idle, NULL);
		}
	}
}

void WorkerData::addToMineralPatch(BWAPI::UnitInterface* unit, int num)
{
    if (workersOnMineralPatch.find(unit) == workersOnMineralPatch.end())
    {
        workersOnMineralPatch[unit] = num;
    }
    else
    {
        workersOnMineralPatch[unit] = workersOnMineralPatch[unit] + num;
    }
}

void WorkerData::setWorkerJob(BWAPI::UnitInterface* unit, enum WorkerJob job, BWAPI::UnitInterface* jobUnit)
{
	if (!unit) { return; }

	clearPreviousJob(unit);
	workerJobMap[unit] = job;

	if (job == Minerals)
	{
		// increase the number of workers assigned to this nexus
		depotWorkerCount[jobUnit] += 1;

		// set the mineral the worker is working on
		workerDepotMap[unit] = jobUnit;

        BWAPI::UnitInterface* mineralToMine = getMineralToMine(unit);
        workerMineralAssignment[unit] = mineralToMine;
        addToMineralPatch(mineralToMine, 1);

		// right click the mineral to start mining
		unit->rightClick(mineralToMine);
	}
	else if (job == Gas)
	{
		// increase the count of workers assigned to this refinery
		refineryWorkerCount[jobUnit] += 1;

		// set the refinery the worker is working on
		workerRefineryMap[unit] = jobUnit;

		// right click the refinery to start harvesting
		unit->rightClick(jobUnit);
	}
    else if (job == Repair)
    {
        // only SCVs can repair
        assert(unit->getType() == BWAPI::UnitTypes::Terran_SCV);

        // set the building the worker is to repair
        workerRepairMap[unit] = jobUnit;

        // start repairing 
        if (!unit->isRepairing())
        {
            unit->repair(jobUnit);
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

void WorkerData::setWorkerJob(BWAPI::UnitInterface* unit, enum WorkerJob job, BWAPI::UnitType jobUnitType)
{
	if (!unit) { return; }

	clearPreviousJob(unit);
	workerJobMap[unit] = job;

	if (job == Build)
	{
		workerBuildingTypeMap[unit] = jobUnitType;
	}
}

void WorkerData::setWorkerJob(BWAPI::UnitInterface* unit, enum WorkerJob job, WorkerMoveData wmd)
{
	if (!unit) { return; }

	clearPreviousJob(unit);
	workerJobMap[unit] = job;

	if (job == Move)
	{
		workerMoveMap[unit] = wmd;
	}

	if (workerJobMap[unit] != Move)
	{
		BWAPI::Broodwar->printf("Something went horribly wrong");
	}
}


void WorkerData::clearPreviousJob(BWAPI::UnitInterface* unit)
{
	if (!unit) { return; }

	WorkerJob previousJob = getWorkerJob(unit);

	if (previousJob == Minerals)
	{
		depotWorkerCount[workerDepotMap[unit]] -= 1;

		workerDepotMap.erase(unit);

        // remove a worker from this unit's assigned mineral patch
        addToMineralPatch(workerMineralAssignment[unit], -1);

        // erase the association from the map
        workerMineralAssignment.erase(unit);
	}
	else if (previousJob == Gas)
	{
		refineryWorkerCount[workerRefineryMap[unit]] -= 1;
		workerRefineryMap.erase(unit);
	}
	else if (previousJob == Build)
	{
		workerBuildingTypeMap.erase(unit);
	}
	else if (previousJob == Repair)
	{
		workerRepairMap.erase(unit);
	}
	else if (previousJob == Move)
	{
		workerMoveMap.erase(unit);
	}

	workerJobMap.erase(unit);
}

int WorkerData::getNumWorkers() const
{
	return workers.size();
}

int WorkerData::getNumMineralWorkers() const
{
	size_t num = 0;
	for (BWAPI::UnitInterface* unit : workers)
	{
		if (workerJobMap.at(unit) == WorkerData::Minerals)
		{
			num++;
		}
	}
	return num;
}

int WorkerData::getNumGasWorkers() const
{
	size_t num = 0;
	for (BWAPI::UnitInterface* unit : workers)
	{
		if (workerJobMap.at(unit) == WorkerData::Gas)
		{
			num++;
		}
	}
	return num;
}

int WorkerData::getNumIdleWorkers() const
{
	size_t num = 0;
	for (BWAPI::UnitInterface* unit : workers)
	{
		if (workerJobMap.at(unit) == WorkerData::Idle)
		{
			num++;
		}
	}
	return num;
}


enum WorkerData::WorkerJob WorkerData::getWorkerJob(BWAPI::UnitInterface* unit)
{
	if (!unit) { return Default; }

	std::map<BWAPI::UnitInterface*, enum WorkerJob>::iterator it = workerJobMap.find(unit);

	if (it != workerJobMap.end())
	{
		return it->second;
	}

	return Default;
}

bool WorkerData::depotIsFull(BWAPI::UnitInterface* depot)
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

std::vector<BWAPI::UnitInterface*> WorkerData::getMineralPatchesNearDepot(BWAPI::UnitInterface* depot)
{
    // if there are minerals near the depot, add them to the set
    std::vector<BWAPI::UnitInterface*> mineralsNearDepot;

    int radius = 300;

    for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->getAllUnits())
	{
		if ((unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field) && unit->getDistance(depot) < radius)
		{
            mineralsNearDepot.push_back(unit);
		}
	}

    // if we didn't find any, use the whole map
    if (mineralsNearDepot.empty())
    {
        for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->getAllUnits())
	    {
		    if ((unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field))
		    {
                mineralsNearDepot.push_back(unit);
		    }
	    }
    }

    return mineralsNearDepot;
}

int WorkerData::getMineralsNearDepot(BWAPI::UnitInterface* depot)
{
	if (!depot) { return 0; }

	int mineralsNearDepot = 0;

	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->getAllUnits())
	{
		if ((unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field) && unit->getDistance(depot) < 200)
		{
			mineralsNearDepot++;
		}
	}

	return mineralsNearDepot;
}

BWAPI::UnitInterface* WorkerData::getWorkerResource(BWAPI::UnitInterface* unit)
{
	if (!unit) { return NULL; }

	// create the iterator
	std::map<BWAPI::UnitInterface*, BWAPI::UnitInterface*>::iterator it;
	
	// if the worker is mining, set the iterator to the mineral map
	if (getWorkerJob(unit) == Minerals)
	{
		it = workerMineralMap.find(unit);
		if (it != workerMineralMap.end())
		{
			return it->second;
		}	
	}
	else if (getWorkerJob(unit) == Gas)
	{
		it = workerRefineryMap.find(unit);
		if (it != workerRefineryMap.end())
		{
			return it->second;
		}	
	}

	return NULL;
}


BWAPI::UnitInterface* WorkerData::getMineralToMine(BWAPI::UnitInterface* worker)
{
	if (!worker) { return NULL; }


	// get the depot associated with this unit
	BWAPI::UnitInterface* depot = getWorkerDepot(worker);
	BWAPI::UnitInterface* bestMineral = NULL;
	double bestDist = 100000;
    double bestNumAssigned = 10000;

	if (depot)
	{
        std::vector<BWAPI::UnitInterface*> mineralPatches = getMineralPatchesNearDepot(depot);

		for (BWAPI::UnitInterface* mineral : mineralPatches)
		{
				double dist = mineral->getDistance(depot);
                double numAssigned = workersOnMineralPatch[mineral];

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
BWAPI::UnitInterface* WorkerData::getMineralToMine(BWAPI::UnitInterface* worker)
{
	if (!worker) { return NULL; }

	// get the depot associated with this unit
	BWAPI::UnitInterface* depot = getWorkerDepot(worker);
	BWAPI::UnitInterface* mineral = NULL;
	double closestDist = 10000;

	if (depot)
	{
		BOOST_FOREACH (BWAPI::UnitInterface* unit, BWAPI::Broodwar->getAllUnits())
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

BWAPI::UnitInterface* WorkerData::getWorkerRepairUnit(BWAPI::UnitInterface* unit)
{
	if (!unit) { return NULL; }

	std::map<BWAPI::UnitInterface*, BWAPI::UnitInterface*>::iterator it = workerRepairMap.find(unit);

	if (it != workerRepairMap.end())
	{
		return it->second;
	}	

	return NULL;
}

BWAPI::UnitInterface* WorkerData::getWorkerDepot(BWAPI::UnitInterface* unit)
{
	if (!unit) { return NULL; }

	std::map<BWAPI::UnitInterface*, BWAPI::UnitInterface*>::iterator it = workerDepotMap.find(unit);

	if (it != workerDepotMap.end())
	{
		return it->second;
	}	

	return NULL;
}

BWAPI::UnitType	WorkerData::getWorkerBuildingType(BWAPI::UnitInterface* unit)
{
	if (!unit) { return BWAPI::UnitTypes::None; }

	std::map<BWAPI::UnitInterface*, BWAPI::UnitType>::iterator it = workerBuildingTypeMap.find(unit);

	if (it != workerBuildingTypeMap.end())
	{
		return it->second;
	}	

	return BWAPI::UnitTypes::None;
}

WorkerMoveData WorkerData::getWorkerMoveData(BWAPI::UnitInterface* unit)
{
	std::map<BWAPI::UnitInterface*, WorkerMoveData>::iterator it = workerMoveMap.find(unit);

	assert(it != workerMoveMap.end());
	
	return (it->second);
}

int WorkerData::getNumAssignedWorkers(BWAPI::UnitInterface* unit)
{
	if (!unit) { return 0; }

	std::map<BWAPI::UnitInterface*, int>::iterator it;
	
	// if the worker is mining, set the iterator to the mineral map
	if (unit->getType().isResourceDepot())
	{
		it = depotWorkerCount.find(unit);

		// if there is an entry, return it
		if (it != depotWorkerCount.end())
		{
			return it->second;
		}
	}
	else if (unit->getType().isRefinery())
	{
		it = refineryWorkerCount.find(unit);

		// if there is an entry, return it
		if (it != refineryWorkerCount.end())
		{
			return it->second;
		}
		// otherwise, we are only calling this on completed refineries, so set it
		else
		{
			refineryWorkerCount[unit] = 0;
		}
	}

	// when all else fails, return 0
	return 0;
}

char WorkerData::getJobCode(BWAPI::UnitInterface* unit)
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
	for (BWAPI::UnitInterface* depot : depots)
	{
		int x = depot->getPosition().x - 64;
		int y = depot->getPosition().y - 32;

		if (Config::Debug::DrawWorkerInfo) BWAPI::Broodwar->drawBoxMap(x-2, y-1, x+75, y+14, BWAPI::Colors::Black, true);
		if (Config::Debug::DrawWorkerInfo) BWAPI::Broodwar->drawTextMap(x, y, "\x04 Workers: %d", getNumAssignedWorkers(depot));

        std::vector<BWAPI::UnitInterface*> minerals = getMineralPatchesNearDepot(depot);

        for (size_t m(0); m<minerals.size(); ++m)
        {
            BWAPI::UnitInterface* mineral = minerals[m];

            int x = mineral->getPosition().x;
		    int y = mineral->getPosition().y;

            if (workersOnMineralPatch.find(mineral) != workersOnMineralPatch.end())
            {
                 //if (Config::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawBoxMap(x-2, y-1, x+75, y+14, BWAPI::Colors::Black, true);
                 //if (Config::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextMap(x, y, "\x04 Workers: %d", workersOnMineralPatch[mineral]);
            }
        }
	}
}