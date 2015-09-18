#pragma once

#include "Common.h"
#include "Array.hpp"
#include "BuildingData.h"
#include "ActionType.h"
#include "ActionInProgress.h"
#include "HatcheryData.h"

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
    HatcheryData                        _hatcheryData;

    ActionsInProgress	                _progress;					
    BuildingData		                _buildings;

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
    const FrameCountType    getActionInProgressFinishTimeByIndex(const UnitCountType & index) const;

    const FrameCountType    getNextBuildingFinishTime() const;
    const FrameCountType    getFinishTime(const ActionType & action) const;
    const FrameCountType    getFinishTime(const PrerequisiteSet & set) const;
    const FrameCountType    getFinishTimeByIndex(const UnitCountType & index) const;
    const FrameCountType    getNextActionFinishTime() const;
    const FrameCountType    getLastActionFinishTime() const;
    //const FrameCountType    getTimeUntilBuildingFree(const ActionType & action) const;

    const FrameCountType    getWhenBuildingCanBuild(const ActionType & action) const;

    const SupplyCountType   getCurrentSupply() const;
    const SupplyCountType   getMaxSupply() const;
    
    void                    setCurrentSupply(const UnitCountType & supply);
    void                    setBuildingWorker();
    void                    releaseBuildingWorker();
    void                    addCompletedBuilding(const ActionType & action, const FrameCountType timeUntilFree, const ActionType & constructing, const ActionType & addon, const int numLarva = 1);
    void                    addCompletedAction(const ActionType & action, bool wasBuilt = true);
	void                    removeCompletedAction(const ActionType & action);
    void                    addActionInProgress(const ActionType & action, const FrameCountType & completionFrame, bool queueAction = true);
    void                    setBuildingFrame(const FrameCountType & frame);
    void                    setMineralWorkers(const UnitCountType & mineralWorkers);
    void                    setGasWorkers(const UnitCountType & gasWorkers);
    void                    setBuildingWorkers(const UnitCountType & buildingWorkers);
    void                    morphUnit(const ActionType & from, const ActionType & to, const FrameCountType & completionFrame);

    ActionType              finishNextActionInProgress();

    const BuildingData &    getBuildingData() const;
    const HatcheryData &    getHatcheryData() const;
          HatcheryData &    getHatcheryData();
};

}