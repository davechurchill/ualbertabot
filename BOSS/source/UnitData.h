#pragma once

#include "Common.h"
#include "Array.hpp"
#include "BuildingData.h"
#include "ActionType.h"
#include "ActionInProgress.h"

namespace BOSS
{

class UnitData
{
    RaceID                              _race;

    UnitCountType                       _mineralWorkers; 		    // number of workers currently mining
    UnitCountType                       _gasWorkers; 			    // number of workers currently getting gas
    UnitCountType                       _buildingWorkers;
    
    SupplyCountType		                _maxSupply; 			    // our maximum allowed supply
    SupplyCountType                     _currentSupply; 			// our current allocated supply
    
    Vec<UnitCountType, Constants::MAX_ACTIONS>     _numUnits;                  // how many of each unit are completed
    Vec<Hatchery, Constants::MAX_HATCHERIES>       _hatcheries;
    UnitCountType                       _numLarva;

    ActionsInProgress	                _progress;					
    BuildingData		                _buildings;

    void                                raceSpecificFinishAction(const ActionType & a);

public:

    UnitData(const RaceID race);

    const RaceID            getRace() const;
    const bool              hasActionsInProgress() const;
    const bool              hasPrerequisites(const PrerequisiteSet & required) const;
    const bool              hasGasIncome() const;
    const bool              hasMineralIncome() const;

    const PrerequisiteSet   getPrerequistesInProgress(const ActionType & action) const;
    
    const UnitCountType     getNumTotal(const ActionType & action) const;
    const UnitCountType     getNumInProgress(const ActionType & action) const;
    const UnitCountType     getNumCompleted(const ActionType & action) const;
    const UnitCountType     getNumActionsInProgress() const;
    const UnitCountType     getNumLarva() const;
    const UnitCountType     getNumMineralWorkers() const;
    const UnitCountType     getNumGasWorkers() const;
    const UnitCountType     getNumBuildingWorkers() const;
    const UnitCountType     getSupplyInProgress() const;

    const ActionType &      getActionInProgressByIndex(const UnitCountType & index) const;

    const FrameCountType    getNextBuildingFinishTime() const;
    const FrameCountType    getFinishTime(const ActionType & action) const;
    const FrameCountType    getFinishTime(const PrerequisiteSet & set) const;
    const FrameCountType    getFinishTimeByIndex(const UnitCountType & index) const;
    const FrameCountType    getNextActionFinishTime() const;
    const FrameCountType    getLastActionFinishTime() const;
    const FrameCountType    getTimeUntilBuildingFree(const ActionType & action) const;

    const FrameCountType    getWhenBuildingCanBuild(const ActionType & action) const;

    const SupplyCountType   getCurrentSupply() const;
    const SupplyCountType   getMaxSupply() const;
    
    void                    setCurrentSupply(const UnitCountType & supply);
    void                    setBuildingWorker();
    void                    releaseBuildingWorker();
    void                    addCompletedBuilding(const ActionType & action, const FrameCountType timeUntilFree, const ActionType & constructing, const ActionType & addon);
    void                    addCompletedAction(const ActionType & action);
    void                    addActionInProgress(const ActionType & action, const FrameCountType & completionFrame, bool queueAction = true);
    void                    setBuildingFrame(const FrameCountType & frame);
    void                    setMineralWorkers(const UnitCountType & mineralWorkers);
    void                    setGasWorkers(const UnitCountType & gasWorkers);
    void                    setBuildingWorkers(const UnitCountType & buildingWorkers);
    void                    setLarva(const UnitCountType & larva);

    void                    finishNextActionInProgress();
};

}