#include "GameState.h"

using namespace BOSS;


GameState::GameState(const RaceID r)
    : _race                 (r)
    , _units                (r)
    , _currentFrame         (0)
    , _lastActionFrame      (0)
    , _minerals             (0)
    , _gas                  (0)
{
    
}

#ifdef _MSC_VER
GameState::GameState(BWAPI::GameWrapper & game, BWAPI::PlayerInterface * self, const std::vector<BWAPI::UnitType> & buildingsQueued)
    : _race                 (Races::GetRaceID(self->getRace()))
    , _currentFrame         (game->getFrameCount())
    , _lastActionFrame      (0)
    , _units                (Races::GetRaceID(self->getRace()))
    , _minerals             (self->minerals() * Constants::RESOURCE_SCALE)
    , _gas                  (self->gas() * Constants::RESOURCE_SCALE)
{ 
    // we will count the worker jobs as we add units
    UnitCountType mineralWorkerCount    = 0;
    UnitCountType gasWorkerCount        = 0;
    UnitCountType buildingWorkerCount   = 0;
    UnitCountType larvaCount            = 0;

    _units.setMineralWorkers(mineralWorkerCount);
    _units.setGasWorkers(gasWorkerCount);
    _units.setBuildingWorkers(buildingWorkerCount);

    // add buildings queued like they had just been started
    for (const BWAPI::UnitType & type : buildingsQueued)
    {
        _units.addActionInProgress(ActionType(type), game->getFrameCount() + type.buildTime(), false);
    }

	// add each unit we have to the current state
	for (BWAPI::UnitInterface * unit : self->getUnits())
	{
        if (unit->getType() == BWAPI::UnitTypes::Zerg_Egg)
        {
            
            _units.addActionInProgress(ActionType(unit->getBuildType()), game->getFrameCount() + unit->getRemainingTrainTime(), false);
            continue;
        }

		if (unit->getType() == BWAPI::UnitTypes::Zerg_Larva)
		{
			++larvaCount;
			continue;
		}

		if (!ActionTypes::TypeExists(unit->getType()))
		{
			continue;
		}
        
        if (unit->getType().isWorker())
        {
            const BWAPI::UnitCommand & command = unit->getLastCommand();

            if (command.getType() == BWAPI::UnitCommandTypes::Build)
            {
                int a = 6;
            }

            if (command.getType() == BWAPI::UnitCommandTypes::Morph)
            {
                int a = 6;
            }
        }

        const ActionType actionType(unit->getType());

		// if the unit is completed
		if (unit->isCompleted())
		{
			// if it is a building
			if (unit->getType().isBuilding() && !unit->getType().isAddon())
			{
                // add the building data accordingly
				FrameCountType  trainTime = unit->getRemainingTrainTime() + unit->getRemainingResearchTime() + unit->getRemainingUpgradeTime();
                ActionType      constructing;
                ActionType      addon;
                std::string     unitName = unit->getType().getName();
                int             numLarva = unit->getLarva().size();
                bool            isHatchery = unit->getType().isResourceDepot() && unit->getType().getRace() == BWAPI::Races::Zerg;

                // if this is a hatchery subtract the training time which is just larva production time
                if (isHatchery)
                {
                    trainTime -= unit->getRemainingTrainTime();
                }

                if (unit->getAddon() && unit->getAddon()->isBeingConstructed())
                {
                    constructing = ActionType(unit->getAddon()->getType());
                } 
                else if (!isHatchery && unit->getRemainingTrainTime() > 0)
                {
                    int remaining = unit->getRemainingTrainTime();
                    auto & queue = unit->getTrainingQueue();

                    auto & first = queue.begin();
                    BWAPI::UnitType typeTraining = *first;
                    constructing = ActionType(typeTraining);
                }
				else if (unit->getRemainingResearchTime() > 0)
				{
					constructing = ActionType(unit->getTech());
					_units.addActionInProgress(constructing, game->getFrameCount() + unit->getRemainingResearchTime(), false);
				}
				else if (unit->getRemainingUpgradeTime() > 0)
				{
					constructing = ActionType(unit->getUpgrade());
					_units.addActionInProgress(constructing, game->getFrameCount() + unit->getRemainingUpgradeTime(), false);
				}

                // TODO: special case for Zerg_Hatchery
                if (unit->getAddon() != NULL)
                {
                    if (unit->getAddon()->isConstructing())
                    {
                        constructing = ActionType(unit->getAddon()->getType());
                    }
                    else
                    {
                        addon = ActionType(unit->getAddon()->getType());    
                    }
                }

                _units.addCompletedBuilding(actionType, trainTime, constructing, addon, numLarva);
			}
            else
            {
                // add the unit to the state
			    _units.addCompletedAction(actionType, false);

                _units.setCurrentSupply(_units.getCurrentSupply() + actionType.supplyRequired());
            }
		}
		else if (unit->isBeingConstructed() && !unit->getType().isAddon())
		{
			_units.addActionInProgress(ActionType(unit->getType()), game->getFrameCount() + unit->getRemainingBuildTime(), false);
		}
	}

    // TODO: set correct number of larva from state
    for (const BWAPI::UpgradeType & type : BWAPI::UpgradeTypes::allUpgradeTypes())
	{
        if (!ActionTypes::TypeExists(type))
		{
			continue;
		}

		if (self->getUpgradeLevel(type) > 0)
		{
			_units.addCompletedAction(ActionType(type));
		}
	}
    
    for (const BWAPI::TechType & type : BWAPI::TechTypes::allTechTypes())
	{
        if (!ActionTypes::TypeExists(type))
		{
			continue;
		}

		if (self->hasResearched(type))
		{
		    _units.addCompletedAction(ActionType(type));
		}
	}

    Logger::LogAppendToFile(BOSS_LOGFILE, toString());
}
#endif

void GameState::setStartingState()
{
    _minerals = 50 * Constants::RESOURCE_SCALE;
    _gas = 0;

    _units.addCompletedAction(ActionTypes::GetResourceDepot(getRace()), false);
    _units.addCompletedAction(ActionTypes::GetWorker(getRace()), false);
    _units.addCompletedAction(ActionTypes::GetWorker(getRace()), false);
    _units.addCompletedAction(ActionTypes::GetWorker(getRace()), false);
    _units.addCompletedAction(ActionTypes::GetWorker(getRace()), false);

    if (getRace() == Races::Zerg)
    {
        _units.addCompletedAction(ActionTypes::GetSupplyProvider(Races::Zerg), false);
    }

    _units.setCurrentSupply(8);
}

const RaceID GameState::getRace() const
{
    return _race;
}

void GameState::getAllLegalActions(ActionSet & actions) const
{
    const std::vector<ActionType> & allActions = ActionTypes::GetAllActionTypes(getRace());
	for (ActionID i(0); i<allActions.size(); ++i)
	{
        const ActionType & action = allActions[i];

        if (isLegal(action))
        {
            actions.add(action);
        }
    }   
}

bool GameState::isLegal(const ActionType & action) const
{
    const size_t numRefineries  = _units.getNumTotal(ActionTypes::GetRefinery(getRace()));
    const size_t numDepots      = _units.getNumTotal(ActionTypes::GetResourceDepot(getRace()));
    const size_t refineriesInProgress = _units.getNumInProgress(ActionTypes::GetRefinery(getRace()));

    // we can never build a larva
    static const ActionType & Zerg_Larva = ActionTypes::GetActionType("Zerg_Larva");
    if (action == Zerg_Larva)
    {
        return false;
    }

    // check if the tech requirements are met
    if (!_units.hasPrerequisites(action.getPrerequisites()))
    {
        return false;
    }
	
    // if it's a unit and we are out of supply and aren't making an overlord, it's not legal
	if (!action.isMorphed() && !action.isSupplyProvider() && ((_units.getCurrentSupply() + action.supplyRequired()) > (_units.getMaxSupply() + _units.getSupplyInProgress())))
    {
        return false;
    }

    // TODO: require an extra for refineries byt not buildings
    // rules for buildings which are built by workers
    if (action.isBuilding() && !action.isMorphed() && !action.isAddon())
    {
        // be very strict about when we can make refineries to ensure we have enough workers to go in gas
        if (action.isRefinery() && (getNumMineralWorkers() <= (4 + 3*refineriesInProgress)))
        {
            return false;
        }

        int workersPerRefinery = 3;
        int workersRequiredToBuild = getRace() == Races::Protoss ? 0 : 1;
        int buildingIsRefinery = action.isRefinery() ? 1 : 0;
        int candidateWorkers = getNumMineralWorkers() + _units.getNumInProgress(ActionTypes::GetWorker(getRace())) + getNumBuildingWorkers();
        int workersToBeUsed = workersRequiredToBuild + workersPerRefinery*(refineriesInProgress);

        if (candidateWorkers < workersToBeUsed)
        {
            return false;
        }
    }

    // if we have no gas income we can't make a gas unit
    if (!canAffordGas(action) && !_units.hasGasIncome())
    {
        return false;
    }

    // if we have no mineral income we'll never have a minerla unit
    if (!canAffordMinerals(action) && !_units.hasMineralIncome())
    {
        return false;
    }

    // don't build more refineries than resource depots
    if (action.isRefinery() && (numRefineries >= numDepots))
    {
        return false;
    }

    // we don't need to go over the maximum supply limit with supply providers
    if (action.isSupplyProvider() && (_units.getMaxSupply() + _units.getSupplyInProgress() > 400))
    {
        return false;
    }

    // can only build one of a tech type
    if (action.isTech() && getUnitData().getNumTotal(action) > 0)
    {
        return false;
    }

    // check to see if an addon can ever be built
    if (action.isAddon() && !_units.getBuildingData().canBuildEventually(action) && (_units.getNumInProgress(action.whatBuildsActionType()) == 0))
    {
        return false;
    }

    return true;
}

// do an action, action must be legal for this not to break
std::vector<ActionType> GameState::doAction(const ActionType & action)
{
    BOSS_ASSERT(action.getRace() == _race, "Race of action does not match race of the state");

    _actionsPerformed.push_back(ActionPerformed());
    _actionsPerformed[_actionsPerformed.size()-1].actionType = action;

    BOSS_ASSERT(isLegal(action), "Trying to perform an illegal action: %s %s", action.getName().c_str(), getActionsPerformedString().c_str());
    
    // set the actionPerformed
    _actionPerformed = action;
    _actionPerformedK = 1;

    FrameCountType workerReadyTime = whenWorkerReady(action);
    FrameCountType ffTime = whenCanPerform(action);

    BOSS_ASSERT(ffTime >= 0 && ffTime < 1000000, "FFTime is very strange: %d", ffTime);

    auto actionsFinished=fastForward(ffTime);

    _actionsPerformed[_actionsPerformed.size()-1].actionQueuedFrame = _currentFrame;
    _actionsPerformed[_actionsPerformed.size()-1].gasWhenQueued = _gas;
    _actionsPerformed[_actionsPerformed.size()-1].mineralsWhenQueued = _minerals;

    // how much time has elapsed since the last action was queued?
    FrameCountType elapsed(_currentFrame - _lastActionFrame);
    _lastActionFrame = _currentFrame;

    BOSS_ASSERT(canAffordMinerals(action),   "Minerals less than price: %ld < %d, ffTime=%d %s", _minerals, action.mineralPrice(), (int)elapsed, action.getName().c_str());
    BOSS_ASSERT(canAffordGas(action),       "Gas less than price: %ld < %d, ffTime=%d %s", _gas, action.gasPrice(), (int)elapsed, action.getName().c_str());

    // modify our resources
    _minerals   -= action.mineralPrice();
    _gas        -= action.gasPrice();

    // do race specific things here
    if (getRace() == Races::Protoss)
    {
        _units.addActionInProgress(action, _currentFrame + action.buildTime());    
    }
    else if (getRace() == Races::Terran)
    {
        if (action.isBuilding() && !action.isAddon())
        {
            if (getNumMineralWorkers() == 0)
            {
                std::cout << toString() << std::endl;
            }

            BOSS_ASSERT(getNumMineralWorkers() > 0, "Don't have any mineral workers to assign");
            _units.setBuildingWorker();
        }

        _units.addActionInProgress(action, _currentFrame + action.buildTime());
    }
    else if (getRace() == Races::Zerg)
    {
     	//  zerg must subtract a larva if the action was unit creation
    	if (action.isUnit() && !action.isBuilding()) 
        {
            if (action.isMorphed())
            {
                _units.morphUnit(action.whatBuildsActionType(), action, _currentFrame + action.buildTime());   
            }
            else
            {
                BOSS_ASSERT(getHatcheryData().numLarva() > 0, "We should have a larva to use");
                _units.getHatcheryData().useLarva();
                _units.addActionInProgress(action, _currentFrame + action.buildTime());
            }
     	}
     	else if (action.isBuilding())
     	{
            _units.morphUnit(action.whatBuildsActionType(), action, _currentFrame + action.buildTime());
     	}
        else
        {
            // if it's not a unit or a building it's a tech so we queue it normally
            _units.addActionInProgress(action, _currentFrame + action.buildTime());
        }
     }

	return actionsFinished;
}

// fast forwards the current state to time toFrame
std::vector<ActionType> GameState::fastForward(const FrameCountType toFrame)
{
    // fast forward the building timers to the current frame
    FrameCountType previousFrame = _currentFrame;
    _units.setBuildingFrame(toFrame - _currentFrame);

    // update resources & finish each action
    FrameCountType      lastActionFinished  = _currentFrame;
    FrameCountType      totalTime           = 0;
    ResourceCountType   moreGas             = 0;
    ResourceCountType   moreMinerals        = 0;


	std::vector<ActionType> actionsFinished;
    // while we still have units in progress
    while ((_units.getNumActionsInProgress() > 0) && (_units.getNextActionFinishTime() <= toFrame))
    {
        // figure out how long since the last action was finished
        FrameCountType timeElapsed 	= _units.getNextActionFinishTime() - lastActionFinished;
        totalTime 			+= timeElapsed;

        // update our mineral and gas count for that period
        moreMinerals 		+= timeElapsed * getMineralsPerFrame();
        moreGas 			+= timeElapsed * getGasPerFrame();

        // update when the last action was finished
        lastActionFinished 	= _units.getNextActionFinishTime();

        // finish the action, which updates mineral and gas rates if required
		actionsFinished.push_back(_units.finishNextActionInProgress());
    }

    // update resources from the last action finished to toFrame
    FrameCountType elapsed  =  toFrame - lastActionFinished;
    moreMinerals            += elapsed * getMineralsPerFrame();
    moreGas                 += elapsed * getGasPerFrame();
    totalTime               += elapsed;

    _minerals               += moreMinerals;
    _gas                    += moreGas;

    // we are now in the FUTURE... "the future, conan?"
    _currentFrame           = toFrame;

    if (getRace() == Races::Zerg)
    {
        _units.getHatcheryData().fastForward(previousFrame, toFrame);
    }

	return actionsFinished;
}

// returns the time at which all resources to perform an action will be available
const FrameCountType GameState::whenCanPerform(const ActionType & action) const
{
    // the resource times we care about
    FrameCountType mineralTime  (_currentFrame); 	// minerals
    FrameCountType gasTime      (_currentFrame); 	// gas
    FrameCountType classTime    (_currentFrame); 	// class-specific
    FrameCountType supplyTime   (_currentFrame); 	// supply
    FrameCountType prereqTime   (_currentFrame); 	// prerequisites
    FrameCountType workerTime   (_currentFrame);
    FrameCountType maxVal       (_currentFrame);

    // figure out when prerequisites will be ready
    prereqTime      = whenPrerequisitesReady(action);

    // check minerals
    mineralTime     = whenMineralsReady(action);

    // check gas
    gasTime         = whenGasReady(action);

    // race specific timings (Zerg Larva)
    classTime       = raceSpecificWhenReady(action);

    // set when we will have enough supply for this unit
    supplyTime      = whenSupplyReady(action);

    // when will we have a worker ready to build it?
    workerTime      = whenWorkerReady(action);

    // figure out the max of all these times
    maxVal = (mineralTime > maxVal) ? mineralTime   : maxVal;
    maxVal = (gasTime >     maxVal) ? gasTime       : maxVal;
    maxVal = (classTime >   maxVal) ? classTime     : maxVal;
    maxVal = (supplyTime >  maxVal) ? supplyTime    : maxVal;
    maxVal = (prereqTime >  maxVal) ? prereqTime    : maxVal;
    maxVal = (workerTime >  maxVal) ? workerTime    : maxVal;

    // return the time
    return maxVal;
}

const FrameCountType GameState::raceSpecificWhenReady(const ActionType & a) const
{
    if (getRace() == Races::Zerg)
    {
        if (getHatcheryData().numLarva() == 0)
        {
            return getHatcheryData().nextLarvaFrameAfter(_currentFrame);
        }
    }

    return 0;
}

const FrameCountType GameState::whenWorkerReady(const ActionType & action) const
{
    if (!action.whatBuildsActionType().isWorker())
    {
        return _currentFrame;
    }

    int refineriesInProgress = _units.getNumInProgress(ActionTypes::GetRefinery(getRace()));

    // protoss doesn't tie up a worker to build, so they can build whenever a mineral worker is free
    if (getRace() == Races::Protoss && getNumMineralWorkers() > 0)
    {
        return _currentFrame;
    }

    // if we have a mineral worker, then it is ready right now
    if (getNumMineralWorkers() > 3*refineriesInProgress)
    {
        return _currentFrame;
    }
    
    // at this point we need to wait for the next worker to become free since existing workers
    // are either all used, or they are reserved to be put into refineries
    // so we must have either a worker in progress, or a building in progress
    const ActionType & Worker = ActionTypes::GetWorker(getRace());
    BOSS_ASSERT(_units.getNumInProgress(Worker) > 0 || getNumBuildingWorkers() > 0, "No worker will ever be free");

    FrameCountType workerReadyTime = _currentFrame;

    // if we have a worker in progress, when will it be ready?
    FrameCountType whenWorkerInProgressFinished = std::numeric_limits<FrameCountType>::max();
    if (_units.getNumInProgress(Worker))
    {
        whenWorkerInProgressFinished = _units.getFinishTime(Worker);
    }

    // if we have a worker currently building, when will it be free?
    FrameCountType whenBuildingWorkerFree = std::numeric_limits<FrameCountType>::max();
    if (getNumBuildingWorkers() > 0)
    {
        whenBuildingWorkerFree = _units.getNextBuildingFinishTime();
    }

    return std::min(whenWorkerInProgressFinished, whenBuildingWorkerFree);
}

const FrameCountType GameState::whenSupplyReady(const ActionType & action) const
{
    int supplyNeeded = action.supplyRequired() + _units.getCurrentSupply() - _units.getMaxSupply();
    if (supplyNeeded <= 0)
    {
        return getCurrentFrame();
    }

    FrameCountType whenSupplyReady = _currentFrame;

    if (supplyNeeded > 0)
    {
        FrameCountType min = 99999;

        // if we don't have the resources, this action would only be legal if there is an
        // overlord in progress, so check to see when the first overlord will finish
        for (int i(0); i<_units.getNumActionsInProgress(); ++i)
        {
            // so, if the unit provides the supply we need
            if (_units.getActionInProgressByIndex(i).supplyProvided() > supplyNeeded)
            {
                // set 'min' to the min of these times
                min = (_units.getFinishTimeByIndex(i) < min) ? _units.getFinishTimeByIndex(i) : min;
            }

            // then set supply time to min
            whenSupplyReady = min;
        }
    }

    return whenSupplyReady;
}

const FrameCountType GameState::whenPrerequisitesReady(const ActionType & action) const
{
    if (action == ActionTypes::GetActionType("Protoss_Dark_Templar"))
    {
        int a = 6;
    }

    FrameCountType preReqReadyTime = _currentFrame;

    // if a building builds this action
    if (action.whatBuildsIsBuilding())
    {
        // get when the building / prereqs will be ready
        preReqReadyTime = whenBuildingPrereqReady(action);
    }
    // otherwise something else builds this action so we don't worry about buildings
    else
    {
        // if requirement in progress (and not already made), set when it will be finished
        PrerequisiteSet reqInProgress = _units.getPrerequistesInProgress(action);

        // if it's not empty, check when they will be done
        if (!reqInProgress.isEmpty())
        {
            preReqReadyTime = _units.getFinishTime(reqInProgress);
        }
    }

    return preReqReadyTime;
}

const FrameCountType GameState::whenBuildingPrereqReady(const ActionType & action) const
{
    FrameCountType buildingAvailableTime(0);
    const ActionType & builder = action.whatBuildsActionType();

    BOSS_ASSERT(builder.isBuilding(), "The thing that builds this is not a building");

    bool buildingIsConstructed                  = _units.getBuildingData().canBuildEventually(action);//getNumCompleted(builder) > 0;
    bool buildingInProgress                     = _units.getNumInProgress(builder) > 0;
    FrameCountType constructedBuildingFreeTime  = std::numeric_limits<int>::max()-10;
    FrameCountType buildingInProgressFinishTime = std::numeric_limits<int>::max()-10;

    BOSS_ASSERT(buildingIsConstructed || (!action.requiresAddon() && buildingInProgress), "We will never be able to build action: %s", action.getName().c_str());
    
    if (buildingIsConstructed)
    {
        constructedBuildingFreeTime  = _currentFrame + _units.getBuildingData().getTimeUntilCanBuild(action);
    }
        
    if (!action.requiresAddon() && buildingInProgress)
    {
        buildingInProgressFinishTime = _units.getFinishTime(builder);
    }

    // this will give us when the building will be free to build this action
    buildingAvailableTime = std::min(constructedBuildingFreeTime, buildingInProgressFinishTime);

    // get all prerequisites currently in progress but do not have any completed
    PrerequisiteSet prereqInProgress = _units.getPrerequistesInProgress(action);

    // remove the specific builder from this list since we calculated that earlier
    prereqInProgress.remove(builder);

    //// if we actually have some prerequisites in progress other than the building
    if (!prereqInProgress.isEmpty())
    {
        // get the max time the earliest of each type will be finished in
        FrameCountType C = _units.getFinishTime(prereqInProgress);

        // take the maximum of this value and when the building was available
        buildingAvailableTime = (C > buildingAvailableTime) ? C : buildingAvailableTime;
    }
    
    return buildingAvailableTime;
}

//const FrameCountType GameState::whenConstructedBuildingReady(const ActionType & builder) const
//{
//    // if what builds a is a building and we have at least one of them completed so far
//    if (builder.isBuilding() && _units.getNumTotal(builder) > 0)
//    {
//        FrameCountType returnTime = _currentFrame + _units.getTimeUntilBuildingFree(builder);
//
//        // get when the next building is available
//        return returnTime;
//    }
//
//    return getCurrentFrame();
//}

// when will minerals be ready
const FrameCountType GameState::whenMineralsReady(const ActionType & action) const
{
    if (_minerals >= action.mineralPrice())
    {
        return getCurrentFrame();
    }
    
    UnitCountType currentMineralWorkers     = _units.getNumMineralWorkers();
    UnitCountType currentGasWorkers         = _units.getNumGasWorkers();
    FrameCountType lastActionFinishFrame    = _currentFrame;
    FrameCountType addedTime                = 0;
    ResourceCountType addedMinerals         = 0;
    ResourceCountType difference            = action.mineralPrice() - _minerals;

    // loop through each action in progress, adding the minerals we would gather from each interval
    for (size_t i(0); i<_units.getNumActionsInProgress(); ++i)
    {
        // the vector is sorted in descending order
        size_t progressIndex = _units.getNumActionsInProgress() - i - 1;

        // the time elapsed and the current minerals per frame
        FrameCountType elapsed = _units.getFinishTimeByIndex(progressIndex) - lastActionFinishFrame;
        ResourceCountType mineralsPerFrame = (currentMineralWorkers * Constants::MPWPF);

        // the amount of minerals that would be added this time step
        ResourceCountType tempAdd = elapsed * mineralsPerFrame;

        // if this amount isn't enough, update the amount added for this interval
        if (addedMinerals + tempAdd < difference)
        {
            addedMinerals += tempAdd;
            addedTime += elapsed;
        }
        else
        {
            // otherwise we can just break out and update at the end
            break;
        }

        // if it was a drone or extractor update the temp variables
        const ActionType & actionPerformed = _units.getActionInProgressByIndex(progressIndex);

        // finishing a building as terran gives you a mineral worker back
        if (actionPerformed.isBuilding() && !actionPerformed.isAddon() && (getRace() == Races::Terran))
        {
            currentMineralWorkers++;
        }

        if (actionPerformed.isWorker())
        {
            currentMineralWorkers++;
        }
        else if (actionPerformed.isRefinery())
        {
            BOSS_ASSERT(currentMineralWorkers > 3, "Not enough mineral workers \n%s", toString().c_str());
            currentMineralWorkers -= 3; 
            currentGasWorkers += 3;
        }


        // update the last action
        lastActionFinishFrame = _units.getFinishTimeByIndex(progressIndex);
    }

    // if we still haven't added enough minerals, add more time
    if (addedMinerals < difference)
    {
        BOSS_ASSERT(currentMineralWorkers > 0, "Shouldn't have 0 mineral workers");

       FrameCountType finalTimeToAdd;
	   if (currentMineralWorkers != 0)
		{
			finalTimeToAdd = (difference - addedMinerals) / (currentMineralWorkers * Constants::MPWPF);
		}
		else
		{
			finalTimeToAdd = 1000000;
		}
        addedMinerals += finalTimeToAdd * currentMineralWorkers * Constants::MPWPF;
        addedTime     += finalTimeToAdd;

        // the last operation could have added one frame too little due to integer division so we need to check
        if (addedMinerals < difference)
        {
            addedTime += 1;
            addedMinerals += currentMineralWorkers * Constants::MPWPF;
        }
    }
    
    BOSS_ASSERT(addedMinerals >= difference, "Mineral prediction error");

    // for some reason if i don't return +1, i mine 1 less mineral in the interval
    return _currentFrame + addedTime;
}

const FrameCountType GameState::whenGasReady(const ActionType & action) const
{
    if (_gas >= action.gasPrice())
    {
        return getCurrentFrame();
    }
    
    UnitCountType currentMineralWorkers     = _units.getNumMineralWorkers();
    UnitCountType currentGasWorkers         = _units.getNumGasWorkers();
    FrameCountType lastActionFinishFrame    = _currentFrame;
    FrameCountType addedTime                = 0;
    ResourceCountType addedGas              = 0;
    ResourceCountType difference            = action.gasPrice() - _gas;

    // loop through each action in progress, adding the minerals we would gather from each interval
    for (size_t i(0); i<_units.getNumActionsInProgress(); ++i)
    {
        // the vector is sorted in descending order
        size_t progressIndex = _units.getNumActionsInProgress() - i - 1;

        // the time elapsed and the current minerals per frame
        FrameCountType elapsed = _units.getFinishTimeByIndex(progressIndex) - lastActionFinishFrame;
        ResourceCountType gasPerFrame = (currentGasWorkers * Constants::GPWPF);

        // the amount of minerals that would be added this time step
        ResourceCountType tempAdd = elapsed * gasPerFrame;

        // if this amount isn't enough, update the amount added for this interval
        if (addedGas + tempAdd < difference)
        {
            addedGas += tempAdd;
            addedTime += elapsed;
        }
        else
        {
            // otherwise we can just break out and update at the end
            break;
        }

        // if it was a drone or extractor update the temp variables
        const ActionType & actionPerformed = _units.getActionInProgressByIndex(progressIndex);

        // finishing a building as terran gives you a mineral worker back
        if (actionPerformed.isBuilding() && !actionPerformed.isAddon() && (getRace() == Races::Terran))
        {
            currentMineralWorkers++;
        }

        if (actionPerformed.isWorker())
        {
            currentMineralWorkers++;
        }
        else if (actionPerformed.isRefinery())
        {
            BOSS_ASSERT(currentMineralWorkers > 3, "Not enough mineral workers");
            currentMineralWorkers -= 3; currentGasWorkers += 3;
        }

        // update the last action
        lastActionFinishFrame = _units.getFinishTimeByIndex(progressIndex);
    }

    // if we still haven't added enough minerals, add more time
    if (addedGas < difference)
    {
		BOSS_ASSERT(currentGasWorkers > 0, "Shouldn't have 0 gas workers");
		FrameCountType finalTimeToAdd;
		if (currentGasWorkers != 0)
		{
			finalTimeToAdd = (difference - addedGas) / (currentGasWorkers * Constants::GPWPF);
		}
		else
		{
			finalTimeToAdd = 1000000;
		}
        addedGas    += finalTimeToAdd * currentGasWorkers * Constants::GPWPF;
        addedTime   += finalTimeToAdd;

        // the last operation could have added one frame too little due to integer division so we need to check
        if (addedGas < difference)
        {
            addedTime += 1;
            addedGas += currentGasWorkers * Constants::GPWPF;
        }
    }
    
    BOSS_ASSERT(addedGas >= difference, "Gas prediction error");

    // for some reason if i don't return +1, i mine 1 less mineral in the interval
    return _currentFrame + addedTime;
}

const FrameCountType GameState::getCurrentFrame() const
{
    return _currentFrame;
}

const FrameCountType GameState::getLastActionFinishTime() const
{
    return _units.getLastActionFinishTime();
}

bool GameState::canAfford(const ActionType & action) const
{
    return canAffordMinerals(action) && canAffordGas(action);
}

bool GameState::canAffordGas(const ActionType & action) const
{
    return _gas >= action.gasPrice();
}

bool GameState::canAffordMinerals(const ActionType & action) const
{
    return _minerals >= action.mineralPrice();
}

// getter methods for the internal variables
size_t GameState::getMineralsPerFrame() const
{
    return Constants::MPWPF * _units.getNumMineralWorkers();
}

size_t GameState::getGasPerFrame() const
{
    return Constants::GPWPF * _units.getNumGasWorkers();
}

const UnitCountType GameState::getNumMineralWorkers() const
{
    return _units.getNumMineralWorkers();
}

const UnitCountType GameState::getNumBuildingWorkers() const
{
    return _units.getNumBuildingWorkers();
}

const UnitCountType GameState::getNumGasWorkers() const
{
    return _units.getNumGasWorkers();
}

const ResourceCountType GameState::getMinerals() const
{
    return _minerals;

}

const ResourceCountType GameState::getGas() const
{
    return _gas;
}

const ResourceCountType GameState::getMinerals(const int frame) const
{
    BOSS_ASSERT(frame >= _currentFrame, "Frame is not in the future");

    return _minerals + (int)(getMineralsPerFrame() * (frame-_currentFrame));
}

const ResourceCountType GameState::getGas(const int frame) const
{
    BOSS_ASSERT(frame >= _currentFrame, "Frame is not in the future");

    return _gas + (int)(getGasPerFrame() * (frame-_currentFrame));
}

const ResourceCountType GameState::getFinishTimeMinerals() const
{
    return getMinerals(_units.getLastActionFinishTime());
}

const ResourceCountType GameState::getFinishTimeGas() const
{
    return getGas(_units.getLastActionFinishTime());
}

const UnitData & GameState::getUnitData() const
{
    return _units;
}

const BuildingData & GameState::getBuildingData() const
{
    return _units.getBuildingData();
}

const HatcheryData & GameState::getHatcheryData() const
{
    return _units.getHatcheryData();
}

void GameState::setMinerals(const ResourceCountType & minerals)
{
    _minerals = minerals * Constants::RESOURCE_SCALE;
}

void GameState::setGas(const ResourceCountType & gas)
{
    _gas = gas * Constants::RESOURCE_SCALE;
}

void GameState::addCompletedAction(const ActionType & action, const size_t num)
{
    for (size_t i(0); i < num; ++i)
    {
        _units.addCompletedAction(action, false);
        _units.setCurrentSupply(_units.getCurrentSupply() + action.supplyRequired());
    }
}

void GameState::removeCompletedAction(const ActionType & action, const size_t num)
{
	for (size_t i(0); i < num; ++i)
	{
		_units.setCurrentSupply(_units.getCurrentSupply() - action.supplyRequired());
		_units.removeCompletedAction(action);
	}
}

const std::string GameState::toString() const
{
	std::stringstream ss;
	ss << "\n-----------------------------------------------------------\n";
    
	ss << "Current Frame: " << _currentFrame << "(" << (_currentFrame / (60 * 24)) << "m " << ((_currentFrame / 24) % 60) << "s)\n\n";

	ss << "Units Completed:\n\n";
    const std::vector<ActionType> & allActions = ActionTypes::GetAllActionTypes(getRace());
	for (ActionID i(0); i<allActions.size(); ++i)
	{
        const ActionType & action = allActions[i];
        if (_units.getNumCompleted(action) > 0) 
        {
			ss << "\t" << (int)_units.getNumCompleted(action) << "\t" << action.getName() << "\n";
        }
    }

	ss << "\nUnits In Progress:\n\n";
    for (int i(0); i<_units.getNumActionsInProgress(); i++) 
    {
		ss << "\t" << (int)_units.getFinishTimeByIndex(i) << "\t" << _units.getActionInProgressByIndex(i).getName() << "\n";
    }

    /*std::cout << "\nLegal Actions:\n\n";
    ActionSet legalActions;
    getAllLegalActions(legalActions);
    for (UnitCountType a(0); a<legalActions.size(); ++a)
    {
        ss << "\t" << legalActions[a].getName() << "\n";
    }*/

	ss << "\nResources:\n\n";
	ss << "\t" << _minerals / Constants::RESOURCE_SCALE << "\tMinerals\n";
	ss << "\t" << _gas / Constants::RESOURCE_SCALE << "\tGas\n";
	ss << "\t" << _units.getNumMineralWorkers() << "\tMineral Workers\n";
    ss << "\t" << _units.getNumGasWorkers() << "\tGas Workers\n";
    ss << "\t" << _units.getNumBuildingWorkers() << "\tBuilding Workers\n";
    ss << "\n\t" << _units.getCurrentSupply()/2 << " / " << _units.getMaxSupply()/2 << "\tSupply\n";


    ss << "-----------------------------------------------------------\n";
    //printPath();

    return ss.str();
}

const std::string GameState::getActionsPerformedString() const
{
    std::stringstream ss;
    ss << std::endl;
    for (size_t a(0); a<_actionsPerformed.size(); ++a)
    {
        ss << (int)_actionsPerformed[a].actionQueuedFrame << " " << (int)_actionsPerformed[a].mineralsWhenQueued << " " << (int)_actionsPerformed[a].gasWhenQueued << " " << _actionsPerformed[a].actionType.getName() << std::endl;
    }

    return ss.str();
}

bool GameState::whyIsNotLegal(const ActionType & action) const
{
    const size_t numRefineries  = _units.getNumTotal(ActionTypes::GetRefinery(getRace()));
    const size_t numDepots      = _units.getNumTotal(ActionTypes::GetResourceDepot(getRace()));
    const size_t refineriesInProgress = _units.getNumInProgress(ActionTypes::GetRefinery(getRace()));

    // we can never build a larva
    static const ActionType & Zerg_Larva = ActionTypes::GetActionType("Zerg_Larva");
    if (action == Zerg_Larva)
    {
        std::cout << "WhyNotLegal: " << action.getName() << " - Cannot build a Larva" << std::endl;
        return false;
    }

    // check if the tech requirements are met
    if (!_units.hasPrerequisites(action.getPrerequisites()))
    {
        std::cout << "WhyNotLegal: " << action.getName() << " - Tech requirements not met" << std::endl;
        return false;
    }

    // if it's a unit and we are out of supply and aren't making an overlord, it's not legal
    if (!action.isMorphed() && ((_units.getCurrentSupply() + action.supplyRequired()) > (_units.getMaxSupply() + _units.getSupplyInProgress())))
    {
        std::cout << "WhyNotLegal: " << action.getName() << " - Not enough supply to construct" << std::endl;
        return false;
    }

    // specific rule for never leaving 0 workers on minerals
    if (action.isRefinery() && (getNumMineralWorkers() <= 4))
    {
        std::cout << "WhyNotLegal: " << action.getName() << " - Cannot leave 0 workers on minerals" << std::endl;
        return false;
    }

    // if it's a new building and no drones are available, it's not legal
    if (!action.isMorphed() && action.isBuilding() && (getNumMineralWorkers() <= 1) && (getNumBuildingWorkers() == 0))
    {
        std::cout << "WhyNotLegal: " << action.getName() << " - No building worker available" << std::endl;
        return false;
    }

    // we can't build a building with our last worker
    if (!action.isMorphed() && action.isBuilding() && (getNumMineralWorkers() <= 1 + 3*refineriesInProgress) && (getNumBuildingWorkers() == 0))
    {
        std::cout << "WhyNotLegal: " << action.getName() << " - Can't build with last worker" << std::endl;
        return false;
    }

    // if we have no gas income we can't make a gas unit
    if (!canAffordGas(action) && !_units.hasGasIncome())
    {
        std::cout << "WhyNotLegal: " << action.getName() << " - No gas income for gas unit" << std::endl;
        return false;
    }

    // if we have no mineral income we'll never have a minerla unit
    if (!canAffordMinerals(action) && !_units.hasMineralIncome())
    {
        std::cout << "WhyNotLegal: " << action.getName() << " - No mineral income" << std::endl;
        return false;
    }

    // don't build more refineries than resource depots
    if (action.isRefinery() && (numRefineries >= numDepots))
    {
        std::cout << "WhyNotLegal: " << action.getName() << " - Can't have more refineries than depots" << std::endl;
        return false;
    }

    // we don't need to go over the maximum supply limit with supply providers
    if (action.isSupplyProvider() && (_units.getMaxSupply() + _units.getSupplyInProgress() >= 400))
    {
        std::cout << "WhyNotLegal: " << action.getName() << " - Can't go over max supply bound with providers" << std::endl;
        return false;
    }

    if (action.isTech() && getUnitData().getNumTotal(action) > 0)
    {
        std::cout << "WhyNotLegal: " << action.getName() << " - Can't produce additional copy of tech" << std::endl;
        return false;
    }

    return true;
}