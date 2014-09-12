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

GameState::GameState(BWAPI::Game * game, BWAPI::Player * self)
    : _race                 (Races::GetRaceID(self->getRace()))
    , _currentFrame         (game->getFrameCount())
    , _lastActionFrame      (0)
    , _units                (Races::GetRaceID(self->getRace()))
    , _minerals             (self->minerals())
    , _gas                  (self->gas())
{
    // we will count the worker jobs as we add units
    UnitCountType mineralWorkerCount    = 0;
    UnitCountType gasWorkerCount        = 0;
    UnitCountType buildingWorkerCount   = 0;
    UnitCountType larvaCount            = 0;

    _units.setMineralWorkers(mineralWorkerCount);
    _units.setGasWorkers(gasWorkerCount);
    _units.setBuildingWorkers(buildingWorkerCount);

	int numLarva(0);

	// add each unit we have to the current state
	for (std::set<BWAPI::Unit *>::const_iterator it = self->getUnits().begin(); it != self->getUnits().end(); it++)
	{
        BWAPI::Unit * unit = *it;

		if (unit->getType() == BWAPI::UnitTypes::Zerg_Larva)
		{
			++larvaCount;
			continue;
		}

		if (!ActionTypes::TypeExists(unit->getType()))
		{
			continue;
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

                if (unit->getRemainingTrainTime() > 0)
                {
                    constructing = ActionType(*unit->getTrainingQueue().begin());
                }
				else if (unit->getRemainingResearchTime() > 0)
				{
					constructing = ActionType(unit->getTech());
				}
				else if (unit->getRemainingUpgradeTime() > 0)
				{
					constructing = ActionType(unit->getTech());
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

                _units.addCompletedBuilding(actionType, trainTime, constructing, addon);
			}
            else
            {
                // add the unit to the state
			    _units.addCompletedAction(actionType);

                _units.setCurrentSupply(_units.getCurrentSupply() + actionType.supplyRequired());
            }
		}
		else if (unit->isBeingConstructed() && !unit->getType().isAddon())
		{
			_units.addActionInProgress(ActionType(unit->getType()), game->getFrameCount() + unit->getRemainingBuildTime(), false);
		}
	}

	_units.setLarva(larvaCount);

    for (std::set<BWAPI::UpgradeType>::const_iterator it = BWAPI::UpgradeTypes::allUpgradeTypes().begin(); it != BWAPI::UpgradeTypes::allUpgradeTypes().end(); it++)
	{
        BWAPI::UpgradeType type = *it;
        if (!ActionTypes::TypeExists(type))
		{
			continue;
		}

		if (self->getUpgradeLevel(type) > 0)
		{
			_units.addCompletedAction(ActionType(type));
		}
	}
    
    for (std::set<BWAPI::TechType>::const_iterator it = BWAPI::TechTypes::allTechTypes().begin(); it != BWAPI::TechTypes::allTechTypes().end(); it++)
	{
        BWAPI::TechType type = *it;
        if (!ActionTypes::TypeExists(type))
		{
			continue;
		}

		if (self->hasResearched(type))
		{
		    _units.addCompletedAction(ActionType(type));
		}
	}
}

void GameState::setStartingState()
{
    _minerals = 50;
    _gas = 0;

    _units.addCompletedAction(ActionTypes::GetResourceDepot(getRace()));
    _units.addCompletedAction(ActionTypes::GetWorker(getRace()));
    _units.addCompletedAction(ActionTypes::GetWorker(getRace()));
    _units.addCompletedAction(ActionTypes::GetWorker(getRace()));
    _units.addCompletedAction(ActionTypes::GetWorker(getRace()));

    if (getRace() == Races::Zerg)
    {
        _units.addCompletedAction(ActionTypes::Zerg_Overlord);
        _units.setLarva(3);
    }

    _units.setCurrentSupply(8);
}

const RaceID GameState::getRace() const
{
    return _race;
}

void GameState::getAllLegalActions(ActionSet & actions) const
{
    const ActionSet & allActions = ActionTypes::GetAllActionTypes(getRace());
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
    //std::cout << ACTIONS[a].getName() << ": ";
    const ActionType & workerType = ActionTypes::GetWorker(getRace());
    const size_t numRefineries  = _units.getNumTotal(ActionTypes::GetRefinery(getRace()));
    const size_t numDepots      = _units.getNumTotal(ActionTypes::GetResourceDepot(getRace()));
    const size_t refineriesInProgress = _units.getNumInProgress(ActionTypes::GetRefinery(getRace()));

    // check if the tech requirements are met
    if (!_units.hasPrerequisites(action.getPrerequisites()))
    {
        return false;
    }

    // if it's a unit and we are out of supply and aren't making an overlord, it's not legal
    if ((_units.getCurrentSupply() + action.supplyRequired()) > (_units.getMaxSupply() + _units.getSupplyInProgress()))
    {
        return false;
    }

    // specific rule for never leaving 0 workers on minerals
    if (action.isRefinery() && (getNumMineralWorkers() < 4))
    {
        return false;
    }

    // if it's a new building and no drones are available, it's not legal
    if (action.isBuilding() && (getNumMineralWorkers() <= 1) && (getNumBuildingWorkers() == 0))
    {
        return false;
    }

    // we can't build a building with our last worker
    if (action.isBuilding() && (getNumMineralWorkers() <= 1 + 3*refineriesInProgress) && (getNumBuildingWorkers() == 0))
    {
        return false;
    }

    // if we have no gas income we can't make a gas unit
    if (((action.gasPrice() - _gas) > 0) && !_units.hasGasIncome())
    {
        return false;
    }

    // if we have no mineral income we'll never have a minerla unit
    if (((action.mineralPrice() - _minerals) > 0) && !_units.hasMineralIncome())
    {
        return false;
    }

    // don't build more refineries than resource depots
    if (action.isRefinery() && (numRefineries >= numDepots))
    {
        return false;
    }

    // we don't need to go over the maximum supply limit with supply providers
    if (action.isSupplyProvider() && (_units.getMaxSupply() + _units.getSupplyInProgress() >= 400))
    {
        return false;
    }

    //std::cout << "  LEGAL!\n";
    return true;
}

// do an action, action must be legal for this not to break
void GameState::doAction(const ActionType & action)
{
    BOSS_ASSERT(action.getRace() == _race, "Race of action does not match race of the state");

    _actionsPerformed.push_back(ActionPerformed());
    _actionsPerformed[_actionsPerformed.size()-1].actionType = action;
    BOSS_ASSERT(isLegal(action), "Trying to perform an illegal action: %s %s", action.getName().c_str(), getActionsPerformedString().c_str());
    
    // set the actionPerformed
    _actionPerformed = action;
    _actionPerformedK = 1;

    FrameCountType ffTime = whenCanPerform(action);

    BOSS_ASSERT(ffTime >= 0 && ffTime < 100000, "FFTime is very strange: %d", ffTime);

    fastForward(ffTime);

    _actionsPerformed[_actionsPerformed.size()-1].actionQueuedFrame = _currentFrame;
    _actionsPerformed[_actionsPerformed.size()-1].gasWhenQueued = _gas;
    _actionsPerformed[_actionsPerformed.size()-1].mineralsWhenQueued = _minerals;

    // how much time has elapsed since the last action was queued?
    FrameCountType elapsed(_currentFrame - _lastActionFrame);
    _lastActionFrame = _currentFrame;

    // queue the action
    _units.addActionInProgress(action, _currentFrame + action.buildTime());

    BOSS_ASSERT(_minerals   >= action.mineralPrice(),   "Minerals less than price: %d < %d, ffTime=%d %s", (int)_minerals, (int)action.mineralPrice(), (int)elapsed, getActionsPerformedString().c_str());
    BOSS_ASSERT(_gas        >= action.gasPrice(),       "Gas less than price: %d < %d, ffTime=%d %s", (int)_gas, (int)action.gasPrice(), (int)elapsed, getActionsPerformedString().c_str());

    // modify our resources
    _minerals   -= action.mineralPrice();
    _gas        -= action.gasPrice();

    // do race specific things here, like subtract a larva
    if (getRace() == Races::Terran)
    {
        if (action.isBuilding())
        {
            _units.setBuildingWorker();
        }
    }
    //else if (ACTIONS.getRace() == BWAPI::Races::Zerg)
    //{
    //	if (GSN_DEBUG) printf("\tSpecific Do Action %d\n", ffTime);

    //	// zerg must subtract a larva if the action was unit creation
    //	if (ACTIONS[a].isUnit() && !ACTIONS[a].isBuilding()) 
    //	{		
    //		if (GSN_DEBUG) printf("\t\tZerg Unit - Remove Larva\n");
    //		// subtract the larva
    //		numLarva--;
    //		
    //		// for each of our hatcheries
    //		for (int i(0); i<numHatcheries; ++i) 
    //		{			
    //			// if this hatchery has a larva available
    //			if (hatcheries[i].numLarva > 0) 
    //			{
    //				// if there were 3 larva previously, update lastUsed
    //				if (hatcheries[i].numLarva == 3) hatcheries[i].lastThirdUsed = currentFrame;

    //				// subtract it from this hatchery
    //				hatcheries[i].numLarva--;
    //				
    //				// break out of the hatchery loop
    //				break;
    //			}
    //		}
    //	}
    //	else if (ACTIONS[a].isBuilding())
    //	{
    //		if (GSN_DEBUG) printf("\t\tZerg Building - Remove Drone\n");

    //		// special case of morphed buildings
    //		if (ACTIONS[a].getUnitType() == BWAPI::UnitTypes::Zerg_Lair || ACTIONS[a].getUnitType() == BWAPI::UnitTypes::Zerg_Greater_Spire)
    //		{
    //			// the previous building starts morphing into this one
    //		}
    //		else
    //		{
    //			// otherwise, a drone is subtracted to start construction
    //			addNumUnits(ACTIONS.getWorker(), -1);
    //			addMineralWorkers(-1);
    //			currentSupply -= ACTIONS[ACTIONS.getWorker()].supplyRequired();
    //		}
    //	}

    //	if (GSN_DEBUG) printf("\tSpecific Do Action End\n");
    //}
}

// fast forwards the current state to time toFrame
void GameState::fastForward(const FrameCountType toFrame)
{
    // fast forward the building timers to the current frame
    _units.setBuildingFrame(toFrame - _currentFrame);

    // update resources & finish each action
    FrameCountType      lastActionFinished(_currentFrame);
    FrameCountType      totalTime(0);
    double              moreGas(0);
    double              moreMinerals(0);

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
        _units.finishNextActionInProgress();
    }

    // update resources from the last action finished to toFrame
    FrameCountType elapsed  =  toFrame - lastActionFinished;
    moreMinerals            += elapsed * getMineralsPerFrame();
    moreGas                 += elapsed * getGasPerFrame();
    totalTime               += elapsed;

    _minerals               += (ResourceCountType)(ceil(moreMinerals));
    _gas                    += (ResourceCountType)(ceil(moreGas));

    // we are now in the FUTURE... "the future, conan?"
    _currentFrame           = toFrame;

    //if (ACTIONS.getRace() == BWAPI::Races::Zerg)
    //{
    //	if (GSN_DEBUG) printf("\t\tSpecific Fast Forward\n");

    //	// for each hatchery we have
    //	for (int i(0); i<numHatcheries; ++i) 
    //	{	
    //		// only do this if there's less than 3 larva present
    //		if (hatcheries[i].numLarva < 3) 
    //		{
    //			// how much time will have past since the last larva used
    //			FrameCountType diff = toFrame - hatcheries[i].lastThirdUsed;

    //			// figure out how many larva should have spawned
    //			UnitCountType numLarvaSpawn = (UnitCountType)(diff / Constants::ZERG_LARVA_TIMER);

    //			// cut that number down to make sure no more than 3 total exist
    //			if (numLarvaSpawn > (3 - hatcheries[i].numLarva)) numLarvaSpawn = 3 - hatcheries[i].numLarva;

    //			// add that to the number of larva
    //			hatcheries[i].numLarva += numLarvaSpawn;
    //			numLarva += numLarvaSpawn;

    //			// if it's less than 3, update the timer ticker
    //			if (hatcheries[i].numLarva < 3) 
    //			{
    //				hatcheries[i].lastThirdUsed += (numLarvaSpawn * Constants::ZERG_LARVA_TIMER);
    //			}
    //			
    //			// if it's exactly 3, set it to toFrame (this might not even matter)
    //			if (hatcheries[i].numLarva == 3) 
    //			{
    //				hatcheries[i].lastThirdUsed = toFrame;
    //			}

    //			//printf("Num Larva Spawn: %d %d\n", numLarvaSpawn, hatcheries[i].numLarva);
    //		} 
    //		else 
    //		{
    //			hatcheries[i].lastThirdUsed = toFrame;
    //		}	
    //	}
    //}
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

    // figure out the max of all these times
    maxVal = (mineralTime > maxVal) ? mineralTime   : maxVal;
    maxVal = (gasTime >     maxVal) ? gasTime       : maxVal;
    maxVal = (classTime >   maxVal) ? classTime     : maxVal;
    maxVal = (supplyTime >  maxVal) ? supplyTime    : maxVal;
    maxVal = (prereqTime >  maxVal) ? prereqTime    : maxVal;

    // return the time
    return maxVal;
}

const FrameCountType GameState::raceSpecificWhenReady(const ActionType & a) const
{
    // PROTOSS: Nothing
    // TERRAN:	If we have 1 or less mineral workers and a worker is building, when will it be free?
    if (getRace() == Races::Terran)
    {
        if (getNumMineralWorkers() <= 1 && getNumBuildingWorkers() > 0)
        {
            return _units.getNextBuildingFinishTime();
        }
    }

    //if (ACTIONS.getRace() == BWAPI::Races::Zerg)
    //{
    //	if (GSN_DEBUG) printf("\tSpecific When Ready\n");
    //	FrameCountType l = currentFrame;		
    //	
    //	// if the current action is a unit
    //	if (ACTIONS[a].isUnit()) 
    //	{
    //		// if we don't have any larva ready, figure out when it will pop
    //           if (units.getNumLarva() == 0)
    //		{
    //			if (GSN_DEBUG) printf("\t\tZerg Larva Not Ready\n");

    //			// min value placeholder
    //			int min = 88888888;

    //			// figure out the hatchery with the least recent used larva
    //			for (int i(0); i<numHatcheries; ++i) 
    //			{
    //				// set the minimum accordingly
    //				min = hatcheries[i].lastThirdUsed < min ? hatcheries[i].lastThirdUsed : min;
    //			}

    //			l = min + Constants::ZERG_LARVA_TIMER + 5;
    //		}
    //	}

    //	if (GSN_DEBUG) printf("\tSpecific When Ready End (return %d)\n", l);
    //	return l;
    //}

    return 0;
}

const FrameCountType GameState::whenSupplyReady(const ActionType & action) const
{
    if (GSN_DEBUG) printf("\tWhen Supply Ready\n");
    FrameCountType s = _currentFrame;

    // set when we will have enough supply for this unit
    int supplyNeeded = action.supplyRequired() + _units.getCurrentSupply() - _units.getMaxSupply();
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
            s = min;
        }
    }

    return s;
}

const FrameCountType GameState::whenPrerequisitesReady(const ActionType & action) const
{
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

    bool buildingIsConstructed                  = _units.getNumCompleted(builder) > 0;
    bool buildingInProgress                     = _units.getNumInProgress(builder) > 0;
    FrameCountType constructedBuildingFreeTime  = INT_MAX-10;
    FrameCountType buildingInProgressFinishTime = INT_MAX-10;

    BOSS_ASSERT(buildingIsConstructed || (!action.requiresAddon() && buildingInProgress), "We will never be able to build action: %s", action.getName().c_str());

    if (buildingIsConstructed)
    {
        constructedBuildingFreeTime  = _currentFrame + _units.getWhenBuildingCanBuild(action);
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

    // if we actually have some prerequisites in progress other than the building
    if (!prereqInProgress.isEmpty())
    {
        // get the max time the earliest of each type will be finished in
        FrameCountType C = _units.getFinishTime(prereqInProgress);

        // take the maximum of this value and when the building was available
        buildingAvailableTime = (C > buildingAvailableTime) ? C : buildingAvailableTime;
    }

    return buildingAvailableTime;
}

const FrameCountType GameState::whenConstructedBuildingReady(const ActionType & builder) const
{
    // if what builds a is a building and we have at least one of them completed so far
    if (builder.isBuilding() && _units.getNumTotal(builder) > 0)
    {
        FrameCountType returnTime = _currentFrame + _units.getTimeUntilBuildingFree(builder);

        // get when the next building is available
        return returnTime;
    }

    return _currentFrame;
}

// when will minerals be ready
const FrameCountType GameState::whenMineralsReady(const ActionType & action) const
{
    ResourceCountType difference = action.mineralPrice() - _minerals;

    double m = _currentFrame;
    double addMinerals = 0;
    double addTime = 0;

    if (difference > 0)
    {
        FrameCountType lastAction = _currentFrame;
        int tmw = _units.getNumMineralWorkers();
        int tgw = _units.getNumGasWorkers();

        for (int i(0); i<_units.getNumActionsInProgress(); ++i)
        {
            // the vector is sorted in descending order
            int ri = _units.getNumActionsInProgress() - i - 1;

            // the time elapsed and the current minerals per frame
            int elapsed = _units.getFinishTimeByIndex(ri) - lastAction;
            double mpf = (tmw * Constants::MPWPF);

            // the amount of minerals that would be added this time step
            double tempAdd = elapsed * mpf;
            double tempTime = elapsed;

            // if this amount is enough to push us over what we need
            if (addMinerals + tempAdd >= difference)
            {
                // figure out when we go over
                tempTime = (difference - addMinerals) / mpf;

                // add the minerals and time
                addMinerals += tempTime * mpf;
                addTime += tempTime;

                //if (GSN_DEBUG) printf("Necessary Minerals Acquired Mid-Iteration: %lf\n", addMinerals); 

                // break out of the loop
                break;

                // otherwise, add the whole interval
            }
            else
            {
                addMinerals += tempAdd;
                addTime += elapsed;

                //if (GSN_DEBUG) printf("Another Mineral Iteration Necessary: %lf\n", addMinerals);
            }

            // if it was a drone or extractor update the temp variables
            if (_units.getActionInProgressByIndex(ri).isWorker())
            {
                tmw++;
            }
            else if (_units.getActionInProgressByIndex(ri).isRefinery())
            {
                tmw -= 3; tgw += 3;
            }

            // update the last action
            lastAction = _units.getFinishTimeByIndex(ri);
        }

        // if we still haven't added enough minerals, add more time
        if (addMinerals < difference)
        {
            addTime += (difference - addMinerals) / (tmw * Constants::MPWPF);

            //if (GSN_DEBUG) printf("\t\tNot Enough Minerals, Adding: minerals(%lf) time(%lf)\n", (difference - addMinerals), addTime); 
        }

        m += addTime;
    }

    //if (GSN_DEBUG) printf("\tMinerals Needs Adding: Minerals(%d, %lf) Frames(%lf, %d > %d)\n", difference, addMinerals, addTime, currentFrame, (int)ceil(m));

    // for some reason if i don't return +1, i mine 1 less mineral in the interval
    return (FrameCountType)ceil(m + 1);
}

const FrameCountType GameState::whenGasReady(const ActionType & action) const
{
    double g = _currentFrame;
    double difference = action.gasPrice() - _gas;
    double addGas = 0;
    double addTime = 0;

    if (difference > 0)
    {
        int lastAction = _currentFrame;
        int tmw = _units.getNumMineralWorkers();
        int tgw = _units.getNumGasWorkers();


        for (int i(0); i<_units.getNumActionsInProgress(); ++i)
        {
            // the vector is sorted in descending order
            int ri = _units.getNumActionsInProgress() - i - 1;

            // the time elapsed and the current minerals per frame
            int elapsed = _units.getFinishTimeByIndex(ri) - lastAction;
            double gpf = (tgw * Constants::GPWPF);

            // the amount of minerals that would be added this time step
            double tempAdd = elapsed * gpf;
            double tempTime = elapsed;

            // if this amount is enough to push us over what we need
            if (addGas + tempAdd >= difference)
            {
                // figure out when we go over
                tempTime = (difference - addGas) / gpf;

                // add the minerals and time
                addGas += tempTime * gpf;
                addTime += tempTime;

                // break out of the loop
                break;

                // otherwise, add the whole interval
            }
            else {

                addGas += tempAdd;
                addTime += elapsed;
            }

            // if it was a drone or extractor update temp variables
            if (_units.getActionInProgressByIndex(ri).isWorker())
            {
                tmw++;
            }
            else if (_units.getActionInProgressByIndex(ri).isRefinery())
            {
                tmw -= 3; tgw += 3;
            }

            // update the last action
            lastAction = _units.getFinishTimeByIndex(ri);
        }

        // if we still haven't added enough minerals, add more time
        if (addGas < difference)
        {
            addTime += (difference - addGas) / (tgw * Constants::GPWPF);
        }

        g += addTime;
    }

    //if (GSN_DEBUG) printf("\tGas Needs Adding: Gas(%d, %lf) Frames(%lf, %d > %d)\n", difference, addGas, addTime, currentFrame, (int)ceil(g));

    return (FrameCountType)(ceil(g) + 1);
}

const FrameCountType GameState::getCurrentFrame() const
{
    return _currentFrame;
}

const FrameCountType GameState::getLastActionFinishTime() const
{
    return _units.getLastActionFinishTime();
}

// getter methods for the internal variables
double GameState::getMineralsPerFrame() const
{
    return Constants::MPWPF * _units.getNumMineralWorkers();
}

double GameState::getGasPerFrame() const
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

const std::string GameState::toString() const
{
    std::cout << "\n-----------------------------------------------------------\n";
    
    std::cout << "Current Frame: " << _currentFrame << "(" << (_currentFrame / 24) << "m " << ((_currentFrame / 24) % 60) << "s)\n\n";

    std::cout << "Units Completed:\n\n";
    const ActionSet & allActions = ActionTypes::GetAllActionTypes(getRace());
	for (ActionID i(0); i<allActions.size(); ++i)
	{
        const ActionType & action = allActions[i];
        if (_units.getNumCompleted(action) > 0) 
        {
            std::cout << "\t" << (int)_units.getNumCompleted(action) << "\t" << action.getName() << "\n";
        }
    }

    std::cout << "\nUnits In Progress:\n\n";
    for (int i(0); i<_units.getNumActionsInProgress(); i++) 
    {
        std::cout << "\t" << (int)_units.getFinishTimeByIndex(i) << "\t" << _units.getActionInProgressByIndex(i).getName() << "\n";
    }

    std::cout << "\nLegal Actions:\n\n";
    ActionSet legalActions;
    getAllLegalActions(legalActions);
    for (UnitCountType a(0); a<legalActions.size(); ++a)
    {
        std::cout << "\t" << legalActions[a].getName() << "\n";
    }

    std::cout << "\nResources:\n\n";
    std::cout << "\t" << _minerals << "\tMinerals\n";
    std::cout << "\t" << _gas << "\tGas\n";
    std::cout << "\t" << _units.getNumMineralWorkers() << "\tMineral Workers\n";
    std::cout << "\t" << _units.getNumGasWorkers() << "\tGas Workers\n";
    std::cout << "\n\t" << _units.getCurrentSupply()/2 << " / " << _units.getMaxSupply()/2 << "\tSupply\n";


    std::cout << "-----------------------------------------------------------\n";
    //printPath();

    return "";
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