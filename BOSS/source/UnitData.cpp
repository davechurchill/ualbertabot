#include "UnitData.h"

using namespace BOSS;

UnitData::UnitData(const RaceID race)
    : _race(race)
    , _numUnits(Constants::MAX_ACTIONS, 0)
    , _currentSupply(0)
    , _maxSupply(0)
    , _mineralWorkers(0)
    , _gasWorkers(0)
    , _buildingWorkers(0)
{

}

const RaceID UnitData::getRace() const
{
    return _race;
}

const UnitCountType UnitData::getNumCompleted(const ActionType & action) const
{
    return _numUnits[action.ID()];
}

void UnitData::setCurrentSupply(const UnitCountType & supply)
{
    _currentSupply = supply;
}

const FrameCountType UnitData::getWhenBuildingCanBuild(const ActionType & action) const
{
    return _buildings.getTimeUntilCanBuild(action);
}

// only used for adding existing buildings from a BWAPI Game * object
void UnitData::addCompletedBuilding(const ActionType & action, const FrameCountType timeUntilFree, const ActionType & constructing, const ActionType & addon, const int numLarva)
{
    _numUnits[action.ID()] += action.numProduced();

    _maxSupply += action.supplyProvided();

    // if it's an extractor
	if (action.isRefinery()) 
	{
		// take those workers from minerals and put them into it
		_mineralWorkers -= 3; _gasWorkers += 3;
	}	

    // if it's a building that can produce units, add it to the building data
	if (action.isBuilding() && !action.isSupplyProvider())
	{
		_buildings.addBuilding(action, timeUntilFree, constructing, addon);
	}

    // special case for hatcheries
    if (action.isBuilding() && (action.getUnitType() == BWAPI::UnitTypes::Zerg_Hatchery))
    {
        _hatcheryData.addHatchery(numLarva);
    }
}

void UnitData::addCompletedAction(const ActionType & action, bool wasBuilt)
{
    const static ActionType Lair = ActionTypes::GetActionType("Zerg_Lair");
    const static ActionType Hive = ActionTypes::GetActionType("Zerg_Hive");

    _numUnits[action.ID()] += wasBuilt ? action.numProduced() : 1;

    if (wasBuilt)
    {
        // a lair or hive from a hatchery don't produce additional supply
        if (action != Lair && action != Hive)
        {
            _maxSupply += action.supplyProvided();
        }
    }
    else
    {
        _maxSupply += action.supplyProvided();
    }
    
    if (action.isWorker()) 
	{ 
		_mineralWorkers++;
	}

    // if it's an extractor
	if (action.isRefinery()) 
	{
		// take those workers from minerals and put them into it
		_mineralWorkers -= 3; _gasWorkers += 3;
	}	

    // if it's a building that can produce units, add it to the building data
	if (action.isBuilding() && !action.isSupplyProvider())
	{
        if (!action.isMorphed())
        {
            _buildings.addBuilding(action, ActionTypes::None);
        }
	}

    // special case for hatcheries
    if (action.isBuilding() && (action.getUnitType() == BWAPI::UnitTypes::Zerg_Hatchery))
    {
        _hatcheryData.addHatchery(wasBuilt ? 1 : 3);
    }
}

void UnitData::removeCompletedAction(const ActionType & action)
{
	//Logger::LogAppendToFile(BOSS_LOGFILE, "Unit removed " + action.getName());
	const static ActionType Lair = ActionTypes::GetActionType("Zerg_Lair");
	const static ActionType Hive = ActionTypes::GetActionType("Zerg_Hive");

	_numUnits[action.ID()] -= action.numProduced();


		// a lair or hive from a hatchery don't produce additional supply
	if (action != Lair && action != Hive)
	{
		_maxSupply -= action.supplyProvided();
	}


	if (action.isWorker())
	{
		if (_mineralWorkers > 0)
		{
			_mineralWorkers--;
		}
		else if (_gasWorkers > 0)
		{
			_gasWorkers--;
		}
	}

	// if it's an extractor
	if (action.isRefinery())
	{
		// take those workers from minerals and put them into it
		_mineralWorkers += 3; _gasWorkers -= 3;
	}
	BOSS_ASSERT(_mineralWorkers >= 0, "Can't have negative mineral workers");
	BOSS_ASSERT(_gasWorkers >= 0, "Can't have negative gas workers");
	// if it's a building that can produce units, add it to the building data
	if (action.isBuilding() && !action.isSupplyProvider())
	{
		if (!action.isMorphed())
		{
			_buildings.removeBuilding(action, ActionTypes::None);
		}
	}

	// special case for hatcheries
	if (action.isBuilding() && (action.getUnitType() == BWAPI::UnitTypes::Zerg_Hatchery))
	{
		_hatcheryData.removeHatchery();
	}
}

void UnitData::addActionInProgress(const ActionType & action, const FrameCountType & completionFrame, bool queueAction)
{
    FrameCountType finishTime = (action.isBuilding() && !action.isMorphed()) ? completionFrame + Constants::BUILDING_PLACEMENT : completionFrame;

	// add it to the actions in progress
	_progress.addAction(action, finishTime);
    
    if (!action.isMorphed())
    {
        _currentSupply += action.supplyRequired() * action.numProduced();
    }

    if (queueAction && action.whatBuildsIsBuilding())
	{
		// add it to a free building, which MUST be free since it's called from doAction
		// which must be already fastForwarded to the correct time
		_buildings.queueAction(action);
	}
}

const bool UnitData::hasActionsInProgress() const
{
    return _progress.isEmpty();
}

const SupplyCountType UnitData::getCurrentSupply() const
{
    return _currentSupply;
}

const SupplyCountType UnitData::getMaxSupply() const
{
    return _maxSupply;
}

void UnitData::setBuildingWorker()
{
    BOSS_ASSERT(_mineralWorkers > 0, "Tried to build without a worker");

    _mineralWorkers--;
    _buildingWorkers++;
}

const bool UnitData::hasGasIncome() const
{
    return _gasWorkers > 0 || getNumTotal(ActionTypes::GetRefinery(getRace())) > 0;
}

const bool UnitData::hasMineralIncome() const
{
    return getNumMineralWorkers() > 0 || getNumBuildingWorkers() > 0 || getNumInProgress(ActionTypes::GetWorker(getRace())) > 0;
}

void UnitData::releaseBuildingWorker()
{
    _mineralWorkers++;
    _buildingWorkers--;
}

void UnitData::setMineralWorkers(const UnitCountType & mineralWorkers)
{
    _mineralWorkers = mineralWorkers;
}

void UnitData::setGasWorkers(const UnitCountType & gasWorkers)
{
    _gasWorkers = gasWorkers;
}

void UnitData::setBuildingWorkers(const UnitCountType & buildingWorkers)
{
    _buildingWorkers = buildingWorkers;
}

void UnitData::morphUnit(const ActionType & from, const ActionType & to, const FrameCountType & completionFrame)
{
    BOSS_ASSERT(getNumCompleted(from) > 0, "Must have the unit type to morph it");
    _numUnits[from.ID()]--;
    _currentSupply -= from.supplyRequired();

    if (from.isWorker())
    {
        BOSS_ASSERT(_mineralWorkers > 0, "Need mineral worker");
        _mineralWorkers--;
    }

    addActionInProgress(to, completionFrame);
}

const UnitCountType UnitData::getNumMineralWorkers() const
{
    return _mineralWorkers;
}

const UnitCountType UnitData::getNumGasWorkers() const
{
    return _gasWorkers;
}

const UnitCountType UnitData::getNumBuildingWorkers() const
{
    return _buildingWorkers;
}

ActionType UnitData::finishNextActionInProgress() 
{	
	// get the actionUnit from the progress data
	ActionType action = _progress.nextAction();

	// add the unit to the unit counter
	addCompletedAction(action);
			
	// pop it from the progress vector
	_progress.popNextAction();
			
	if (getRace() == Races::Terran)
	{
		// if it's a building, release the worker back
		if (action.isBuilding() && !action.isAddon())
		{
			releaseBuildingWorker();
		}
	}
	else if (getRace() == Races::Zerg)
	{
        const static ActionType hatchery = ActionTypes::GetActionType("Zerg_Hatchery");

	}

	return action;
}

const FrameCountType UnitData::getNextActionFinishTime() const
{
    return _progress.nextActionFinishTime();
}

const FrameCountType UnitData::getNextBuildingFinishTime() const
{
    return _progress.nextBuildingFinishTime();
}

void UnitData::setBuildingFrame(const FrameCountType & frame)
{
    _buildings.fastForwardBuildings(frame);
}

const UnitCountType UnitData::getNumTotal(const ActionType & action) const
{
    return _numUnits[action.ID()] + (_progress.numInProgress(action) * action.numProduced());
}

const bool UnitData::hasPrerequisites(const PrerequisiteSet & required) const
{
    static const ActionType & Hatchery      = ActionTypes::GetActionType("Zerg_Hatchery");
    static const ActionType & Lair          = ActionTypes::GetActionType("Zerg_Lair");
    static const ActionType & Hive          = ActionTypes::GetActionType("Zerg_Hive");
    static const ActionType & Spire         = ActionTypes::GetActionType("Zerg_Spire");
    static const ActionType & GreaterSpire  = ActionTypes::GetActionType("Zerg_Greater_Spire");

    for (size_t a(0); a<required.size(); ++a)
    {
        const ActionType & type = required.getActionType(a);
        const size_t & req = required.getActionTypeCount(a);
        size_t have = getNumTotal(type);

        // special check for zerg moprhed buildings
        if (_race == Races::Zerg)
        {
            if (type == Hatchery)
            {
                have += getNumTotal(Lair);
                have += getNumTotal(Hive);
            }
            else if (type == Lair)
            {
                have += getNumTotal(Hive);
            }
            else if (type == Spire)
            {
                have += getNumTotal(GreaterSpire);
            }
        }

        if (have < req)
        {
            return false;
        }
    }

    return true;
}

const UnitCountType UnitData::getSupplyInProgress() const
{
    const ActionType & provider(ActionTypes::GetSupplyProvider(getRace()));
    const ActionType & depot(ActionTypes::GetResourceDepot(getRace()));
    
    return _progress.numInProgress(provider) * provider.supplyProvided() + _progress.numInProgress(depot) * depot.supplyProvided();
}

const UnitCountType UnitData::getNumActionsInProgress() const
{
    return _progress.size();
}

const UnitCountType UnitData::getNumInProgress(const ActionType & action) const
{
    return _progress.numInProgress(action);
}

const ActionType & UnitData::getActionInProgressByIndex(const UnitCountType & index) const
{
    return _progress.getAction(index);
}

const FrameCountType UnitData::getActionInProgressFinishTimeByIndex(const UnitCountType & index) const
{
    return _progress.getTime(index);
}

const FrameCountType UnitData::getLastActionFinishTime() const
{
    return _progress.getLastFinishTime();
}

const FrameCountType UnitData::getFinishTimeByIndex(const UnitCountType & index) const
{
    return _progress.getTime(index);
}

const FrameCountType UnitData::getFinishTime(const PrerequisiteSet & set) const
{
    return _progress.whenActionsFinished(set);
}

//const FrameCountType UnitData::getTimeUntilBuildingFree(const ActionType & action) const
//{
//    return _buildings.timeUntilFree(action);
//}

const FrameCountType UnitData::getFinishTime(const ActionType & action) const
{
    return _progress.nextActionFinishTime(action);
}

const PrerequisiteSet UnitData::getPrerequistesInProgress(const ActionType & action) const
{
    PrerequisiteSet inProgress;

    for (size_t a(0); a<action.getPrerequisites().size(); ++a)
    {
        const ActionType & actionType = action.getPrerequisites().getActionType(a);
        if (getNumInProgress(actionType) > 0 && getNumCompleted(actionType) == 0)
        {
            inProgress.add(actionType);
        }
    } 

    return inProgress;
}

const BuildingData & UnitData::getBuildingData() const
{
    return _buildings;
}

HatcheryData & UnitData::getHatcheryData()
{
    return _hatcheryData;
}

const HatcheryData & UnitData::getHatcheryData() const
{
    return _hatcheryData;
}