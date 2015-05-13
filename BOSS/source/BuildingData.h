#pragma once

#include "Common.h"

#include <string.h>
#include <queue>
#include <algorithm>

#include "PrerequisiteSet.h"
#include "Array.hpp"
#include "ActionType.h"

namespace BOSS
{
    
class BuildingStatus
{

public:

	ActionType _type;               // the type of building this is
    FrameCountType _timeRemaining;  // amount of time until the unit is finished constructing
    ActionType _isConstructing;     // the type of unit the building is currently constructing
    ActionType _addon;              // the type of addon that the building currently has (is set once completed)
	
	// the number of frames remaining (from currentFrame) until this building is free
	

	BuildingStatus();
	
	BuildingStatus(const ActionType & t, const ActionType & addon);
	BuildingStatus(const ActionType & t, FrameCountType time, const ActionType & constructing, const ActionType & addon);

    const bool canBuildNow(const ActionType & action) const;
    const bool canBuildEventually(const ActionType & action) const;
    void queueActionType(const ActionType & action);
    void fastForward(const FrameCountType frames);

    const std::string toString() const;
};

class BuildingData
{
	Vec<BuildingStatus, Constants::MAX_BUILDINGS> _buildings;

public:

	BuildingData();

	void addBuilding(const ActionType & action, const ActionType & addon);
	void removeBuilding(const ActionType & action, const ActionType & addon);
	void addBuilding(const ActionType & action, const FrameCountType timeUntilFree, const ActionType & constructing, const ActionType & addon);

	const BuildingStatus & getBuilding(const UnitCountType i) const;
	
	// gets the time until building of type t is free
	// this will only ever be called if t exists, so min will always be set to a lower value
	//FrameCountType timeUntilFree(const ActionType & action) const;

    const FrameCountType getTimeUntilCanBuild(const ActionType & action) const;

	// queue an action
	void queueAction(const ActionType & action);
	void fastForwardBuildings(const FrameCountType frames);
	void printBuildingInformation() const;
    const size_t & size() const;

    const bool canBuildNow(const ActionType & action) const;
    const bool canBuildEventually(const ActionType & action) const;
};
}