#include "BuildingData.h"

using namespace BOSS;

BuildingStatus::BuildingStatus() 
: _type(ActionTypes::None)
, _timeRemaining(0) 
, _isConstructing(ActionTypes::None)
, _addon(ActionTypes::None)
{
	
}
	
BuildingStatus::BuildingStatus(const ActionType & action, const ActionType & addon) 
: _type(action)
, _timeRemaining(0) 
, _isConstructing(ActionTypes::None)
, _addon(addon)
{
}

BuildingStatus::BuildingStatus(const ActionType & action, FrameCountType time, const ActionType & constructing, const ActionType & addon) 
: _type(action)
, _timeRemaining(time) 
, _isConstructing(constructing)
, _addon(addon)
{
}

const bool BuildingStatus::canBuildEventually(const ActionType & action) const
{
    if (!_type.canBuild(action))
    {
        return false;
    }

    // if the type is an addon
    if (action.isAddon())
    {
        // if we already have an addon we can't build it
        if (_addon != ActionTypes::None)
        {
            return false;
        }

        // if we are building an addon we can't ever build it
        if (_timeRemaining > 0 && _isConstructing.isAddon())
        {
            return false;
        }
    }

    if (action.requiresAddon() && (_addon != action.requiredAddonType()))
    {
        if (_isConstructing != action.requiredAddonType())
        {
            return false;
        }
    }

    // if the built type is morphed and we are morphing something, we won't be able to build it
    if (action.isMorphed() && (_timeRemaining > 0) && (_isConstructing.isMorphed()))
    {
        return false;
    }

    return true;
}

const bool BuildingStatus::canBuildNow(const ActionType & action) const
{
    if (_timeRemaining > 0)
    {
        return false;
    }

    if (!_type.canBuild(action))
    {
        return false;
    }

    if (action.isAddon() && (_addon != ActionTypes::None))
    {
        return false;
    }

    if (action.requiresAddon() && (_addon != action.requiredAddonType()))
    {
        return false;
    }


    return true;
}

void BuildingStatus::queueActionType(const ActionType & action)
{
    _timeRemaining = action.buildTime();
    _isConstructing = action;
}

void BuildingStatus::fastForward(const FrameCountType frames)
{
    // if we fastforward more than the current time remaining, we will complete the action
    bool willComplete = _timeRemaining <= frames;
    int timeWasRemaining = _timeRemaining;
    const std::string & name = _type.getName();

    if ((_timeRemaining > 0) && willComplete)
    {
        BOSS_ASSERT(_isConstructing != ActionTypes::None, "We can't be building a unit without a type");

        _timeRemaining = 0;

        // if it's building an addon, add it
        if (_isConstructing.isAddon())
        {
            _addon = _isConstructing;
        }

        // if we are finishing a morphed type, it becomes that type
        if (_isConstructing.isMorphed())
        {
            _type = _isConstructing;
        }

        _isConstructing = ActionTypes::None;
    }
    else if (_timeRemaining > 0)
    {
        _timeRemaining -= frames;
    }
}

BuildingData::BuildingData() 
{
}

const size_t & BuildingData::size() const
{
    return _buildings.size();
}

void BuildingData::addBuilding(const ActionType & action, const ActionType & addon)
{
	BOSS_ASSERT(action.isBuilding(), "Trying to add a non-building to the building data");
	
    _buildings.push_back(BuildingStatus(action, addon));
}

void BuildingData::removeBuilding(const ActionType & action, const ActionType & addon)
{
	BOSS_ASSERT(action.isBuilding(), "Trying to remove a non-building from the building data");

	for (size_t i = 0; i < _buildings.size(); i++)
	{
		if (_buildings[i]._type == action)
		{
			_buildings.remove(i);
			break;
		}
	}
}

void BuildingData::addBuilding(const ActionType & action, const FrameCountType timeUntilFree, const ActionType & constructing, const ActionType & addon)
{
	BOSS_ASSERT(action.isBuilding(), "Trying to add a non-building to the building data");
	
    _buildings.push_back(BuildingStatus(action, timeUntilFree, constructing, addon));
}

const BuildingStatus & BuildingData::getBuilding(const UnitCountType i) const
{
	return _buildings[i];
}

// how long from now until we can build the given action
const FrameCountType BuildingData::getTimeUntilCanBuild(const ActionType & action) const
{
    bool minset = false;
	FrameCountType min = 0;
	
    for (size_t i=0; i<_buildings.size(); ++i)
	{
        if (_buildings[i].canBuildEventually(action))
        {
            if (!minset || _buildings[i]._timeRemaining < min)
            {
                minset = true;
                min = _buildings[i]._timeRemaining;
            }
        }
    }


    BOSS_ASSERT(minset, "Min was not set");
    return min;
}

// gets the time until building of type t is free
// this will only ever be called if t exists, so min will always be set to a lower value
//FrameCountType BuildingData::timeUntilFree(const ActionType & action) const
//{
//    BOSS_ASSERT(_buildings.size() > 0, "Called timeUntilFree on empty building data");
//
//    bool minset = false;
//	FrameCountType min = 0;
//	
//	for (size_t i=0; i<_buildings.size(); ++i)
//	{
//		if (_buildings[i]._type == action && (!minset || _buildings[i]._timeRemaining < min))
//		{
//			min = _buildings[i]._timeRemaining;
//            minset = true;
//		}
//	}
//		
//	BOSS_ASSERT(minset, "No min was set");
//	
//	return min;
//}

void BuildingData::queueAction(const ActionType & action)
{	
	for (size_t i=0; i<_buildings.size(); ++i)
	{
		if (_buildings[i].canBuildNow(action))
		{
			_buildings[i].queueActionType(action);
			return;
		}
	}
		
	// this method should always work since we have fast forwarded to the correct point in time
	BOSS_ASSERT(false, "Didn't find a building to queue this type of unit in: %s", action.getName().c_str());
}
	
// fast forward all the building states by amount: frames
void BuildingData::fastForwardBuildings(const FrameCountType frames)
{
	for (size_t i=0; i<_buildings.size(); ++i)
	{
        _buildings[i].fastForward(frames);
	}
}
	
const bool BuildingData::canBuildNow(const ActionType & action) const
{
    for (size_t i=0; i<_buildings.size(); ++i)
	{
        if (_buildings[i].canBuildNow(action))
        {
            return true;
        }
    }

    return false;
}

const bool BuildingData::canBuildEventually(const ActionType & action) const
{
    for (size_t i=0; i<_buildings.size(); ++i)
	{
        if (_buildings[i].canBuildEventually(action))
        {
            return true;
        }
    }

    return false;
}

void BuildingData::printBuildingInformation() const
{
	for (size_t i=0; i<_buildings.size(); ++i)
	{
		if (_buildings[i]._timeRemaining == 0) 
		{
			printf("BUILDING INFO: %s is free to assign\n", _buildings[i]._type.getName().c_str());
		}
		else 
		{
			printf("BUILDING INFO: %s will be free in %d frames\n", _buildings[i]._type.getName().c_str(), _buildings[i]._timeRemaining);
		}
	}
		
	printf("-----------------------------------------------------------\n\n");
}