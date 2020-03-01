#pragma once

#include "Common.h"

namespace UAlbertaBot
{
class WorkerMoveData
{
public:

    int mineralsNeeded  = 0;
    int gasNeeded       = 0;
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

    enum WorkerJob { Minerals, Gas, Build, Combat, Idle, Repair, Move, Scout, Default };

private:

    BWAPI::Unitset m_workers;
    BWAPI::Unitset m_depots;

    std::map<BWAPI::Unit, enum WorkerJob>   m_workerJobMap;
    std::map<BWAPI::Unit, BWAPI::Unit>      m_workerMineralMap;
    std::map<BWAPI::Unit, BWAPI::Unit>      m_workerDepotMap;
    std::map<BWAPI::Unit, BWAPI::Unit>      m_workerRefineryMap;
    std::map<BWAPI::Unit, BWAPI::Unit>      m_workerRepairMap;
    std::map<BWAPI::Unit, WorkerMoveData>   m_workerMoveMap;
    std::map<BWAPI::Unit, BWAPI::UnitType>  m_workerBuildingTypeMap;

    std::map<BWAPI::Unit, int>              m_depotWorkerCount;
    std::map<BWAPI::Unit, int>              m_refineryWorkerCount;
    std::map<BWAPI::Unit, int>              m_workersOnMineralPatch;
    std::map<BWAPI::Unit, BWAPI::Unit>      m_workerMineralAssignment;

    void clearPreviousJob(BWAPI::Unit unit);

public:

    WorkerData();

    void workerDestroyed(BWAPI::Unit unit);
    void addDepot(BWAPI::Unit unit);
    void removeDepot(BWAPI::Unit unit);
    void addWorker(BWAPI::Unit unit);
    void addWorker(BWAPI::Unit unit, WorkerJob job, BWAPI::Unit jobUnit);
    void addWorker(BWAPI::Unit unit, enum WorkerJob job, BWAPI::UnitType jobUnitType);
    void addToMineralPatch(BWAPI::Unit unit, int num);
    void drawDepotDebugInfo();
    void setWorkerJob(BWAPI::Unit unit, enum WorkerJob job, BWAPI::Unit jobUnit);
    void setWorkerJob(BWAPI::Unit unit, enum WorkerJob job, BWAPI::UnitType jobUnitType);
    void setWorkerJob(BWAPI::Unit unit, enum WorkerJob job, WorkerMoveData wmd);

    int  getNumWorkers() const;
    int  getNumMineralWorkers() const;
    int  getNumGasWorkers() const;
    int  getNumIdleWorkers() const;
    int  getNumAssignedWorkers(BWAPI::Unit unit);
    int  getMineralsNearDepot(BWAPI::Unit depot);
    char getJobCode(BWAPI::Unit unit);
    bool depotIsFull(BWAPI::Unit depot);

    BWAPI::Unit     getMineralToMine(BWAPI::Unit worker);
    enum WorkerJob  getWorkerJob(BWAPI::Unit unit);
    BWAPI::Unit     getWorkerResource(BWAPI::Unit unit);
    BWAPI::Unit     getWorkerDepot(BWAPI::Unit unit);
    BWAPI::Unit     getWorkerRepairUnit(BWAPI::Unit unit);
    BWAPI::UnitType getWorkerBuildingType(BWAPI::Unit unit);
    WorkerMoveData  getWorkerMoveData(BWAPI::Unit unit);
    BWAPI::Unitset  getMineralPatchesNearDepot(BWAPI::Unit depot);

    const BWAPI::Unitset & getWorkers() const;
};
}