#include "Common.h"
#include "GameCommander.h"
#include "UnitUtil.h"
#include "Timer.hpp"
#include "Global.h"
#include "UAlbertaBot_Tournament.h"

using namespace UAlbertaBot;

GameCommander::GameCommander(UAlbertaBot_Tournament & uabModule) 
    : _productionManager(_bossManager)
	, _scoutManager(uabModule.Bases())
    , _initialScoutSet(false)
    , _module(uabModule)
	, _combatCommander(uabModule.Bases())
{
	auto& workerManager = uabModule.Workers();
	_scoutManager.onScoutAssigned([&workerManager](const BWAPI::Unit &unit)
	{
		workerManager.setScoutWorker(unit);
	});
	_scoutManager.onScoutReleased([&workerManager](const BWAPI::Unit &unit)
	{
		workerManager.finishedWithWorker(unit);
	});
}

void GameCommander::onStart()
{
    _productionManager.onStart();
}

void GameCommander::update()
{
	_timer.start();

	// Do nothing if we don't have any enemy.
	auto enemy = Global::getEnemy();
	if (enemy == nullptr)
	{
		return;
	}

	handleUnitAssignments();
    
	_bossManager.update(35 - _timer.getElapsedTimeInMilliSec());

	_productionManager.update();
	_combatCommander.update(_combatUnits);
    _scoutManager.update();
	auto& canvas = _module.getCanvas();
	_scoutManager.drawScoutInformation(canvas, 200, 320);
	drawDebugInterface(canvas);
}

ProductionManager & GameCommander::getProductionManager()
{
    return _productionManager;
}

void GameCommander::drawDebugInterface(AKBot::ScreenCanvas& canvas)
{
	Global::UnitInfo().drawExtendedInterface(canvas);
	Global::UnitInfo().drawUnitInformation(canvas, 425,30);
	_productionManager.drawProductionInformation(canvas, 30, 50);
	_combatCommander.drawSquadInformation(canvas, 200, 30);
    
    drawGameInformation(canvas, 4, 1);

	// draw position of mouse cursor
	if (Config::Debug::DrawMouseCursorInfo)
	{
		int mouseX = BWAPI::Broodwar->getMousePosition().x + BWAPI::Broodwar->getScreenPosition().x;
		int mouseY = BWAPI::Broodwar->getMousePosition().y + BWAPI::Broodwar->getScreenPosition().y;
		canvas.drawTextMap(mouseX + 20, mouseY, " %d %d", mouseX, mouseY);
	}
}

void GameCommander::drawGameInformation(AKBot::ScreenCanvas& canvas, int x, int y)
{
	auto enemy = Global::getEnemy();
	auto self = BWAPI::Broodwar->self();
    canvas.drawTextScreen(x, y, "\x04Players:");
	if (enemy != nullptr)
	{
		canvas.drawTextScreen(x + 50, y, "%c%s \x04vs. %c%s", self->getTextColor(), self->getName().c_str(),
			enemy->getTextColor(), enemy->getName().c_str());
	}

	y += 12;
		
    canvas.drawTextScreen(x, y, "\x04Strategy:");
	canvas.drawTextScreen(x+50, y, "\x03%s %s", Config::Strategy::StrategyName.c_str(), Config::Strategy::FoundEnemySpecificStrategy ? "(enemy specific)" : "");
	canvas.setTextSize();
	y += 12;

    canvas.drawTextScreen(x, y, "\x04Map:");
	canvas.drawTextScreen(x+50, y, "\x03%s", BWAPI::Broodwar->mapFileName().c_str());
	canvas.setTextSize();
	y += 12;

    canvas.drawTextScreen(x, y, "\x04Time:");
    canvas.drawTextScreen(x+50, y, "\x04%d %4dm %3ds", BWAPI::Broodwar->getFrameCount(), (int)(BWAPI::Broodwar->getFrameCount()/(23.8*60)), (int)((int)(BWAPI::Broodwar->getFrameCount()/23.8)%60));
}

// assigns units to various managers
void GameCommander::handleUnitAssignments()
{
	_validUnits.clear();
    _combatUnits.clear();

	// filter our units for those which are valid and usable
	setValidUnits();

	// set each type of unit
	setScoutUnits();
	setCombatUnits();
}

bool GameCommander::isAssigned(BWAPI::Unit unit) const
{
    return     (std::find(_combatUnits.begin(), _combatUnits.end(), unit) != _combatUnits.end())
            || (std::find(_scoutUnits.begin(),  _scoutUnits.end(),  unit) != _scoutUnits.end());
}

// validates units as usable for distribution to various managers
void GameCommander::setValidUnits()
{
	// make sure the unit is completed and alive and usable
	for (auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (UnitUtil::IsValidUnit(unit))
		{	
			_validUnits.push_back(unit);
		}
	}
}

void GameCommander::setScoutUnits()
{
    // if we haven't set a scout unit, do it
    if (_scoutUnits.empty() && !_initialScoutSet)
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
                _scoutManager.setWorkerScout(workerScout);
				assignUnit(workerScout, _scoutUnits);
                _initialScoutSet = true;
			}
		}
    }
}

// sets combat units to be passed to CombatCommander
void GameCommander::setCombatUnits()
{
	for (auto & unit : _validUnits)
	{
		if (!isAssigned(unit) && UnitUtil::IsCombatUnit(unit) || unit->getType().isWorker())		
		{	
			assignUnit(unit, _combatUnits);
		}
	}
}

BWAPI::Unit GameCommander::getFirstSupplyProvider()
{
	BWAPI::Unit supplyProvider = nullptr;
	auto self = BWAPI::Broodwar->self();
	auto selfRace = self->getRace();
	auto supplyProviderUnitType = selfRace == BWAPI::Races::Zerg
		? BWAPI::UnitTypes::Zerg_Spawning_Pool
		: selfRace.getSupplyProvider();

	for (auto & unit : self->getUnits())
	{
		if (unit->getType() == supplyProviderUnitType)
		{
			supplyProvider = unit;
		}
	}

	return supplyProvider;
}

void GameCommander::onUnitShow(BWAPI::Unit unit)			
{ 
	
}

void GameCommander::onUnitHide(BWAPI::Unit unit)			
{ 
	
}

void GameCommander::onUnitCreate(BWAPI::Unit unit)		
{ 
	
}

void GameCommander::onUnitComplete(BWAPI::Unit unit)
{
	
}

void GameCommander::onUnitRenegade(BWAPI::Unit unit)		
{ 
	}

void GameCommander::onUnitDestroy(BWAPI::Unit unit)		
{ 	
	_productionManager.onUnitDestroy(unit);
}

void GameCommander::onUnitMorph(BWAPI::Unit unit)		
{ 
	
}


BWAPI::Unit GameCommander::getClosestUnitToTarget(BWAPI::UnitType type, BWAPI::Position target)
{
	BWAPI::Unit closestUnit = nullptr;
	double closestDist = 100000;

	for (auto & unit : _validUnits)
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

	for (auto & unit : _validUnits)
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

void GameCommander::assignUnit(BWAPI::Unit unit, std::vector<BWAPI::Unit> & set)
{
    if (std::find(_scoutUnits.begin(), _scoutUnits.end(), unit) != _scoutUnits.end())
    {
        _scoutUnits.erase(std::remove(_scoutUnits.begin(), _scoutUnits.end(), unit), _scoutUnits.end());
    }
    else if (std::find(_combatUnits.begin(), _combatUnits.end(), unit) != _combatUnits.end())
    {
        _combatUnits.erase(std::remove(_combatUnits.begin(), _combatUnits.end(), unit), _combatUnits.end());
    }

    set.push_back(unit);
}
