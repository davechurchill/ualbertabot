#include "Common.h"
#include "GameCommander.h"
#include "UnitUtil.h"
#include "BaseLocationManager.h"
#include "Global.h"
#include "BuildingPlacerManager.h"
#include "BOSSManager.h"
#include "MapTools.h"
#include "InformationManager.h"
#include "WorkerManager.h"
#include "ProductionManager.h"
#include "BuildingManager.h"
#include "ScoutManager.h"
#include "StrategyManager.h"
#include "Squad.h"

using namespace UAlbertaBot;

GameCommander::GameCommander() 
    : m_initialScoutSet(false)
{

}

void GameCommander::update()
{
    PROFILE_FUNCTION();

	m_timerManager.startTimer(TimerManager::All);

	// populate the unit vectors we will pass into various managers
	handleUnitAssignments();

	// utility managers
	m_timerManager.startTimer(TimerManager::InformationManager);
	Global::Info().update();
	m_timerManager.stopTimer(TimerManager::InformationManager);

	m_timerManager.startTimer(TimerManager::MapTools);
	Global::Map().onFrame();
	m_timerManager.stopTimer(TimerManager::MapTools);

	// economy and base managers
	m_timerManager.startTimer(TimerManager::Worker);
	Global::Workers().onFrame();
	m_timerManager.stopTimer(TimerManager::Worker);

	m_timerManager.startTimer(TimerManager::Production);
	Global::Production().update();
	m_timerManager.stopTimer(TimerManager::Production);

	// combat and scouting managers
	m_timerManager.startTimer(TimerManager::Combat);
	m_combatCommander.update(m_combatUnits);
	m_timerManager.stopTimer(TimerManager::Combat);

	m_timerManager.startTimer(TimerManager::Scout);
    Global::Scout().update();
	m_timerManager.stopTimer(TimerManager::Scout);

	m_timerManager.stopTimer(TimerManager::All);

	Global::Bases().onFrame();

	drawDebugInterface();
}

void GameCommander::drawDebugInterface()
{
	Global::Info().drawExtendedInterface();
	Global::Info().drawUnitInformation(425,30);
	Global::Info().drawMapInformation();
	
	Global::Production().drawProductionInformation(30, 50);
    
	m_combatCommander.drawSquadInformation(200, 30);
    m_timerManager.displayTimers(490, 225);
    drawGameInformation(4, 1);

	// draw position of mouse cursor
	if (Config::Debug::DrawMouseCursorInfo)
	{
		int mouseX = BWAPI::Broodwar->getMousePosition().x + BWAPI::Broodwar->getScreenPosition().x;
		int mouseY = BWAPI::Broodwar->getMousePosition().y + BWAPI::Broodwar->getScreenPosition().y;
		BWAPI::Broodwar->drawTextMap(mouseX + 20, mouseY, " %d %d", mouseX, mouseY);
	}
}

void GameCommander::drawGameInformation(int x, int y)
{
    BWAPI::Broodwar->drawTextScreen(x, y, "\x04Players:");
	BWAPI::Broodwar->drawTextScreen(x+50, y, "%c%s \x04vs. %c%s", BWAPI::Broodwar->self()->getTextColor(), BWAPI::Broodwar->self()->getName().c_str(), 
                                                                  BWAPI::Broodwar->enemy()->getTextColor(), BWAPI::Broodwar->enemy()->getName().c_str());
	y += 12;
		
    BWAPI::Broodwar->drawTextScreen(x, y, "\x04Strategy:");
	BWAPI::Broodwar->drawTextScreen(x+50, y, "\x03%s %s", Config::Strategy::StrategyName.c_str(), Config::Strategy::FoundEnemySpecificStrategy ? "(enemy specific)" : "");
	BWAPI::Broodwar->setTextSize();
	y += 12;

    BWAPI::Broodwar->drawTextScreen(x, y, "\x04Map:");
	BWAPI::Broodwar->drawTextScreen(x+50, y, "\x03%s", BWAPI::Broodwar->mapFileName().c_str());
	BWAPI::Broodwar->setTextSize();
	y += 12;

    BWAPI::Broodwar->drawTextScreen(x, y, "\x04Time:");
    BWAPI::Broodwar->drawTextScreen(x+50, y, "\x04%d %4dm %3ds", BWAPI::Broodwar->getFrameCount(), (int)(BWAPI::Broodwar->getFrameCount()/(23.8*60)), (int)((int)(BWAPI::Broodwar->getFrameCount()/23.8)%60));
}

// assigns units to various managers
void GameCommander::handleUnitAssignments()
{
	m_validUnits.clear();
    m_combatUnits.clear();

	// filter our units for those which are valid and usable
	setValidUnits();

	// set each type of unit
	setScoutUnits();
	setCombatUnits();
}

bool GameCommander::isAssigned(BWAPI::Unit unit) const
{
	return m_combatUnits.contains(unit) || m_scoutUnits.contains(unit);
}

// validates units as usable for distribution to various managers
void GameCommander::setValidUnits()
{
	// make sure the unit is completed and alive and usable
	for (auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (UnitUtil::IsValidUnit(unit))
		{	
			m_validUnits.insert(unit);
		}
	}
}

void GameCommander::setScoutUnits()
{
    // if we haven't set a scout unit, do it
    if (m_scoutUnits.empty() && !m_initialScoutSet)
    {
        BWAPI::Unit supplyProvider = getFirstSupplyProvider();

		// if it exists
		if (supplyProvider)
		{
			// grab the closest worker to the supply provider to send to scout
			BWAPI::Unit workerScout = getClosestWorkerToTarget(supplyProvider->getPosition());

			// if we find a worker (which we should) add it to the scout units
			if (workerScout)
			{
                Global::Scout().setWorkerScout(workerScout);
				assignUnit(workerScout, m_scoutUnits);
                m_initialScoutSet = true;
			}
		}
    }
}

// sets combat units to be passed to CombatCommander
void GameCommander::setCombatUnits()
{
	for (auto & unit : m_validUnits)
	{
		if (!isAssigned(unit) && UnitUtil::IsCombatUnit(unit) || unit->getType().isWorker())		
		{	
			assignUnit(unit, m_combatUnits);
		}
	}
}

BWAPI::Unit GameCommander::getFirstSupplyProvider()
{
	BWAPI::Unit supplyProvider = nullptr;

	if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
	{
		for (auto & unit : BWAPI::Broodwar->self()->getUnits())
		{
			if (unit->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool)
			{
				supplyProvider = unit;
			}
		}
	}
	else
	{
		
		for (auto & unit : BWAPI::Broodwar->self()->getUnits())
		{
			if (unit->getType() == BWAPI::Broodwar->self()->getRace().getSupplyProvider())
			{
				supplyProvider = unit;
			}
		}
	}

	return supplyProvider;
}

void GameCommander::onUnitShow(BWAPI::Unit unit)			
{ 
	Global::Info().onUnitShow(unit); 
	Global::Workers().onUnitShow(unit);
}

void GameCommander::onUnitHide(BWAPI::Unit unit)			
{ 
	Global::Info().onUnitHide(unit); 
}

void GameCommander::onUnitCreate(BWAPI::Unit unit)		
{ 
	Global::Info().onUnitCreate(unit); 
}

void GameCommander::onUnitComplete(BWAPI::Unit unit)
{
	Global::Info().onUnitComplete(unit);
}

void GameCommander::onUnitRenegade(BWAPI::Unit unit)		
{ 
	Global::Info().onUnitRenegade(unit); 
}

void GameCommander::onUnitDestroy(BWAPI::Unit unit)		
{ 	
	Global::Production().onUnitDestroy(unit);
	Global::Workers().onUnitDestroy(unit);
	Global::Info().onUnitDestroy(unit); 
}

void GameCommander::onUnitMorph(BWAPI::Unit unit)		
{ 
	Global::Info().onUnitMorph(unit);
	Global::Workers().onUnitMorph(unit);
}

BWAPI::Unit GameCommander::getClosestUnitToTarget(BWAPI::UnitType type, BWAPI::Position target)
{
	BWAPI::Unit closestUnit = nullptr;
	double closestDist = 100000;

	for (auto & unit : m_validUnits)
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

BWAPI::Unit GameCommander::getClosestWorkerToTarget(BWAPI::Position target)
{
	BWAPI::Unit closestUnit = nullptr;
	double closestDist = 100000;

	for (auto & unit : m_validUnits)
	{
		if (!isAssigned(unit) && unit->getType().isWorker() && Global::Workers().isFree(unit))
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

void GameCommander::assignUnit(BWAPI::Unit unit, BWAPI::Unitset & set)
{
    if (m_scoutUnits.contains(unit)) { m_scoutUnits.erase(unit); }
    else if (m_combatUnits.contains(unit)) { m_combatUnits.erase(unit); }

    set.insert(unit);
}
