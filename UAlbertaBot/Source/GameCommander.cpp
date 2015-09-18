#include "Common.h"
#include "GameCommander.h"

using namespace UAlbertaBot;

GameCommander::GameCommander() : numWorkerScouts(0), currentScout(NULL)
{

}

void GameCommander::update()
{
	timerManager.startTimer(TimerManager::All);


	// populate the unit vectors we will pass into various managers
	populateUnitVectors();

	// utility managers
	timerManager.startTimer(TimerManager::InformationManager);
	InformationManager::Instance().update();
	timerManager.stopTimer(TimerManager::InformationManager);

	timerManager.startTimer(TimerManager::MapGrid);
	MapGrid::Instance().update();
	timerManager.stopTimer(TimerManager::MapGrid);

	timerManager.startTimer(TimerManager::MapTools);
	//MapTools::Instance().update();
	timerManager.stopTimer(TimerManager::MapTools);

	timerManager.startTimer(TimerManager::Search);
	BOSSManager::Instance().update(35 - timerManager.getTotalElapsed());
	timerManager.stopTimer(TimerManager::Search);

	// economy and base managers
	timerManager.startTimer(TimerManager::Worker);
	WorkerManager::Instance().update();
	timerManager.stopTimer(TimerManager::Worker);

	timerManager.startTimer(TimerManager::Production);
	ProductionManager::Instance().update();
	timerManager.stopTimer(TimerManager::Production);

	timerManager.startTimer(TimerManager::Building);
	BuildingManager::Instance().update();
	timerManager.stopTimer(TimerManager::Building);

	// combat and scouting managers
	timerManager.startTimer(TimerManager::Combat);
	if (Config::Modules::UsingCombatCommander)
	{
		combatCommander.update(combatUnits);
	}
	timerManager.stopTimer(TimerManager::Combat);

	timerManager.startTimer(TimerManager::Scout);
	if (Config::Modules::UsingScoutManager)
	{
		scoutManager.update(scoutUnits);
	}
	timerManager.stopTimer(TimerManager::Scout);
		
	timerManager.stopTimer(TimerManager::All);

	drawDebugInterface();
}

void GameCommander::drawDebugInterface()
{
	timerManager.displayTimers(490, 225);
	
    InformationManager::Instance().drawExtendedInterface();
	BOSSManager::Instance().drawSearchInformation(10, 200);
	BuildingManager::Instance().drawBuildingInformation(200,50);
	ProductionManager::Instance().drawProductionInformation(10, 30);
	InformationManager::Instance().drawUnitInformation(425,30);
	InformationManager::Instance().drawMapInformation();

	combatCommander.drawSquadInformation(200, 30);

	// draw position of mouse cursor
	if (Config::Debug::DrawMouseCursorInfo)
	{
		BWAPI::Broodwar->drawTextScreen(20, 20, "Frame: %7d\nTime: %4dm %3ds", BWAPI::Broodwar->getFrameCount(), BWAPI::Broodwar->getFrameCount()/(24*60), (BWAPI::Broodwar->getFrameCount()/24)%60);
		int mouseX = BWAPI::Broodwar->getMousePosition().x + BWAPI::Broodwar->getScreenPosition().x;
		int mouseY = BWAPI::Broodwar->getMousePosition().y + BWAPI::Broodwar->getScreenPosition().y;
		BWAPI::Broodwar->drawTextMap(mouseX + 20, mouseY, " %d %d", mouseX, mouseY);
	}
}

// assigns units to various managers
void GameCommander::populateUnitVectors()
{
	// filter our units for those which are valid and usable
	setValidUnits();

	// set each type of unit
	setScoutUnits();
	setCombatUnits();
	setWorkerUnits();
}

const bool GameCommander::isAssigned(BWAPI::UnitInterface* unit) const
{
	return assignedUnits.find(unit) != assignedUnits.end();
}

// validates units as usable for distribution to various managers
void GameCommander::setValidUnits()
{
	validUnits.clear();
	assignedUnits.clear();

	// make sure the unit is completed and alive and usable
	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (isValidUnit(unit))
		{	
			validUnits.insert(unit);
		}
	}
}

// selects which units will be scouting
// currently only selects the worker scout after first pylon built
// this does NOT take that worker away from worker manager, but it still works
// TODO: take this worker away from worker manager in a clever way
void GameCommander::setScoutUnits()
{
	// if we have just built our first suply provider, set the worker to a scout
	if (numWorkerScouts == 0)
	{
		// get the first supply provider we come across in our units, this should be the first one we make
		BWAPI::UnitInterface* supplyProvider = getFirstSupplyProvider();

		// if it exists
		if (supplyProvider)
		{
			// grab the closest worker to the supply provider to send to scout
			BWAPI::UnitInterface* workerScout = getClosestWorkerToTarget(supplyProvider->getPosition());

			// if we find a worker (which we should) add it to the scout vector
			if (workerScout)
			{
				numWorkerScouts++;
				scoutUnits.insert(workerScout);
				assignedUnits.insert(workerScout);
				WorkerManager::Instance().setScoutWorker(workerScout);
			}
		}
	}
}

// sets combat units to be passed to CombatCommander
void GameCommander::setCombatUnits()
{
	combatUnits.clear();

	for (BWAPI::UnitInterface* unit : validUnits)
	{
		if (!isAssigned(unit) && isCombatUnit(unit))		
		{	
			combatUnits.insert(unit);
			assignedUnits.insert(unit);
		}
	}

	// emergency situation, enemy is in our base and we have no combat units
	// add our workers to the combat force
    int workersToDefend = StrategyManager::Instance().defendWithWorkers();
	if (combatUnits.empty() && (workersToDefend > 0))
	{
		for (BWAPI::UnitInterface* unit : validUnits)
		{
			// if it's a worker
			if (!isAssigned(unit) && unit->getType().isWorker())			
			{	
				// only assign it to combat if it's not building something
				if (!WorkerManager::Instance().isBuilder(unit))
				{
					combatUnits.insert(unit);
					assignedUnits.insert(unit);
                    workersToDefend--;
				}

                if (workersToDefend <= 0)
                {
                    break;
                }
			}
		}
	}
}

bool GameCommander::isCombatUnit(BWAPI::UnitInterface* unit) const
{
	UAB_ASSERT(unit != NULL, "Unit was null");

	// no workers or buildings allowed
	if (unit && unit->getType().isWorker() || unit->getType().isBuilding())
	{
		return false;
	}

	// check for various types of combat units
	if (unit->getType().canAttack() || 
		unit->getType() == BWAPI::UnitTypes::Terran_Medic ||
		unit->getType() == BWAPI::UnitTypes::Protoss_High_Templar ||
		unit->getType() == BWAPI::UnitTypes::Protoss_Observer)
	{
		return true;
	}
		
	return false;
}

BWAPI::UnitInterface* GameCommander::getFirstSupplyProvider()
{
	BWAPI::UnitInterface* supplyProvider = NULL;

	if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
	{
		for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->self()->getUnits())
		{
			if (unit->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool)
			{
				supplyProvider = unit;
			}
		}
	}
	else
	{
		
		for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->self()->getUnits())
		{
			if (unit->getType() == BWAPI::Broodwar->self()->getRace().getSupplyProvider())
			{
				supplyProvider = unit;
			}
		}
	}

	return supplyProvider;
}

void GameCommander::setWorkerUnits()
{
	workerUnits.clear();

	for (BWAPI::UnitInterface* unit : validUnits)
	{
		if (!isAssigned(unit) && unit->getType().isWorker())			
		{	
			workerUnits.insert(unit);
			assignedUnits.insert(unit);
		}
	}

}

bool GameCommander::isValidUnit(BWAPI::UnitInterface* unit)
{
	if (!unit)
	{
		return false;
	}

	if (unit->isCompleted() 
		&& unit->getHitPoints() > 0 
		&& unit->exists() 
		&& unit->getType() != BWAPI::UnitTypes::Unknown 
		&& unit->getPosition().x != BWAPI::Positions::Unknown.x 
		&& unit->getPosition().y != BWAPI::Positions::Unknown.y) 
	{
		return true;
	}
	else
	{
		return false;
	}
}

void GameCommander::onUnitShow(BWAPI::UnitInterface* unit)			
{ 
	InformationManager::Instance().onUnitShow(unit); 
	WorkerManager::Instance().onUnitShow(unit);
}

void GameCommander::onUnitHide(BWAPI::UnitInterface* unit)			
{ 
	InformationManager::Instance().onUnitHide(unit); 
}

void GameCommander::onUnitCreate(BWAPI::UnitInterface* unit)		
{ 
	InformationManager::Instance().onUnitCreate(unit); 
}

void GameCommander::onUnitComplete(BWAPI::UnitInterface* unit)
{
	InformationManager::Instance().onUnitComplete(unit);
}

void GameCommander::onUnitRenegade(BWAPI::UnitInterface* unit)		
{ 
	InformationManager::Instance().onUnitRenegade(unit); 
}

void GameCommander::onUnitDestroy(BWAPI::UnitInterface* unit)		
{ 	
	ProductionManager::Instance().onUnitDestroy(unit);
	WorkerManager::Instance().onUnitDestroy(unit);
	InformationManager::Instance().onUnitDestroy(unit); 
}

void GameCommander::onUnitMorph(BWAPI::UnitInterface* unit)		
{ 
	InformationManager::Instance().onUnitMorph(unit);
	WorkerManager::Instance().onUnitMorph(unit);
}

BWAPI::UnitInterface* GameCommander::getClosestUnitToTarget(BWAPI::UnitType type, BWAPI::Position target)
{
	BWAPI::UnitInterface* closestUnit = NULL;
	double closestDist = 100000;

	for (BWAPI::UnitInterface* unit : validUnits)
	{
		if (unit->getType() == type)
		{
			double dist = unit->getDistance(target);
			if (!closestUnit || dist < closestDist)
			{
				closestUnit = unit;
				closestDist = dist;
			}
		}
	}

	return closestUnit;
}

BWAPI::UnitInterface* GameCommander::getClosestWorkerToTarget(BWAPI::Position target)
{
	BWAPI::UnitInterface* closestUnit = NULL;
	double closestDist = 100000;

	for (BWAPI::UnitInterface* unit : validUnits)
	{
		if (!isAssigned(unit) && unit->getType().isWorker() && WorkerManager::Instance().isFree(unit))
		{
			double dist = unit->getDistance(target);
			if (!closestUnit || dist < closestDist)
			{
				closestUnit = unit;
				closestDist = dist;
			}
		}
	}

	return closestUnit;
}