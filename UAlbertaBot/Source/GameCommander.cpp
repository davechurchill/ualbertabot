#include "Common.h"
#include "GameCommander.h"
#include "UnitUtil.h"
#include "Timer.hpp"

using namespace UAlbertaBot;

GameCommander::GameCommander(
	shared_ptr<AKBot::OpponentView> opponentView,
	shared_ptr<BOSSManager> bossManager,
	shared_ptr<CombatCommander> combatCommander,
	shared_ptr<ScoutManager> scoutManager,
	shared_ptr<ProductionManager> productionManager,
	shared_ptr<WorkerManager> workerManager)
    : _opponentView(opponentView)
	, _workerManager(workerManager)
	, _productionManager(productionManager)
	, _scoutManager(scoutManager)
    , _initialScoutSet(false)
	, _combatCommander(combatCommander)
	, _bossManager(bossManager)
{
	_scoutManager->onScoutAssigned([this](const BWAPI::Unit &unit, int currentFrame)
	{
		_workerManager->setScoutWorker(unit, currentFrame);
	});
	_scoutManager->onScoutReleased([this](const BWAPI::Unit &unit, int currentFrame)
	{
		_workerManager->finishedWithWorker(unit, currentFrame);
	});
}

void GameCommander::onStart()
{
	_productionManager->onStart();
}

void GameCommander::update(int currentFrame)
{
	_timer.start();
	_workerManager->update(currentFrame);

	// Do nothing if we don't have any enemy.
	handleUnitAssignments(currentFrame);
    
	_bossManager->update(35 - _timer.getElapsedTimeInMilliSec(), currentFrame);

	_productionManager->update(currentFrame);
	_combatCommander->update(getCombatUnits(), currentFrame);
	_scoutManager->update(currentFrame);
}

const shared_ptr<ProductionManager> GameCommander::getProductionManager() const
{
    return _productionManager;
}

const shared_ptr<ScoutManager> GameCommander::getScoutManager() const
{
	return _scoutManager;
}

const shared_ptr<CombatCommander> GameCommander::getCombatCommander() const
{
	return _combatCommander;
}

const shared_ptr<AKBot::OpponentView> GameCommander::getOpponentView() const
{
	return _opponentView;
}

// assigns units to various managers
void GameCommander::handleUnitAssignments(int currentFrame)
{
	_validUnits.clear();
    _combatUnits.clear();

	// filter our units for those which are valid and usable
	setValidUnits();

	// set each type of unit
	setScoutUnits(currentFrame);
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
	for (auto & unit : _opponentView->self()->getUnits())
	{
		if (UnitUtil::IsValidUnit(unit))
		{	
			_validUnits.push_back(unit);
		}
	}
}

void GameCommander::setScoutUnits(int currentFrame)
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
                _scoutManager->setWorkerScout(workerScout, currentFrame);
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
	auto self = _opponentView->self();
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
	_workerManager->onUnitShow(unit);
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

void GameCommander::onUnitDestroy(BWAPI::Unit unit, int currentFrame)
{
	_workerManager->onUnitDestroy(unit, currentFrame);
	_productionManager->onUnitDestroy(unit, currentFrame);
}

void GameCommander::onUnitMorph(BWAPI::Unit unit)		
{
	_workerManager->onUnitMorph(unit);
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
		if (!isAssigned(unit) && unit->getType().isWorker() && _workerManager->isFree(unit))
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
