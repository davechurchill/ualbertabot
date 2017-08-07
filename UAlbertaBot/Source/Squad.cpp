#include "Squad.h"
#include "UnitUtil.h"
#include "Global.h"

using namespace UAlbertaBot;

Squad::Squad(
	const std::string & name,
	SquadOrder order,
	size_t priority,
	AKBot::PlayerLocationProvider& locationProvider,
	shared_ptr<AKBot::OpponentView> opponentView,
	shared_ptr<UnitInfoManager> unitInfo,
	shared_ptr<BaseLocationManager> bases,
	shared_ptr<MapTools> mapTools,
	std::shared_ptr<AKBot::Logger> logger)
	: _name(name)
	, _order(order)
    , _lastRetreatSwitch(0)
    , _lastRetreatSwitchVal(false)
    , _priority(priority)
	, _transportManager(opponentView, bases, locationProvider, mapTools, logger)
	, _opponentView(opponentView)
	, _unitInfo(unitInfo)
	, _meleeManager(opponentView, bases)
	, _medicManager(opponentView, bases)
	, _rangedManager(opponentView, bases)
	, _detectorManager(opponentView, mapTools, bases)
	, _tankManager(opponentView, bases)
	, _logger(logger)
{
}

Squad::~Squad()
{
    //clear();
}

void Squad::update(shared_ptr<MapTools> map, int currentFrame)
{
	// update all necessary unit information within this squad
	updateUnits(map);

	// determine whether or not we should regroup
	bool needToRegroup = needsToRegroup(map, currentFrame);
    
	// draw some debug info
	auto distanceFunction = [&map](const BWAPI::Position & src, const BWAPI::Position & dest)
	{
		return map->getGroundDistance(src, dest);
	};

	// if we do need to regroup, do it
	_needToRegroup = needToRegroup;
	if (needToRegroup)
	{
		BWAPI::Position regroupPosition = calcRegroupPosition();
        
		_meleeManager.regroup(map, regroupPosition, currentFrame);
		_rangedManager.regroup(map, regroupPosition, currentFrame);
        _tankManager.regroup(map, regroupPosition, currentFrame);
        _medicManager.regroup(map, regroupPosition, currentFrame);
		_lastRegroupPosition = regroupPosition;
	}
	else // otherwise, execute micro
	{
		_meleeManager.execute(map, _order, currentFrame);
		_rangedManager.execute(map, _order, currentFrame);
        _tankManager.execute(map, _order, currentFrame);
        _medicManager.execute(map, _order, currentFrame);

		_transportManager.update(map, currentFrame);

		auto closestToEnemyUnit = unitClosestToEnemy(distanceFunction);
		_detectorManager.setUnitClosestToEnemy(closestToEnemyUnit);
		_detectorManager.execute(map, _order, currentFrame);
	}
}

bool Squad::isEmpty() const
{
    return _units.empty();
}

size_t Squad::getPriority() const
{
    return _priority;
}

void Squad::setPriority(const size_t & priority)
{
    _priority = priority;
}

void Squad::updateUnits(shared_ptr<MapTools> map)
{
	setAllUnits();
	setNearEnemyUnits(map);
	addUnitsToMicroManagers();
}

void Squad::setAllUnits()
{
	// clean up the _units vector just in case one of them died
	std::set<BWAPI::Unit> goodUnits;
	for (auto & unit : _units)
	{
		if( unit->isCompleted() && 
			unit->getHitPoints() > 0 && 
			unit->exists() &&
			unit->getPosition().isValid() &&
			unit->getType() != BWAPI::UnitTypes::Unknown)
		{
			goodUnits.insert(unit);
		}
	}
	_units = goodUnits;
}

void Squad::setNearEnemyUnits(shared_ptr<MapTools> map)
{
	_nearEnemy.clear();
	for (auto & unit : _units)
	{
		_nearEnemy[unit] = unitNearEnemy(map, unit);
	}
}

void Squad::addUnitsToMicroManagers()
{
	std::vector<BWAPI::Unit> meleeUnits;
	std::vector<BWAPI::Unit> rangedUnits;
	std::vector<BWAPI::Unit> detectorUnits;
	std::vector<BWAPI::Unit> transportUnits;
    std::vector<BWAPI::Unit> tankUnits;
    std::vector<BWAPI::Unit> medicUnits;

	// add _units to micro managers
	for (auto & unit : _units)
	{
		if(unit->isCompleted() && unit->getHitPoints() > 0 && unit->exists())
		{
			// select dector _units
			auto unitType = unit->getType();
            if (unitType == BWAPI::UnitTypes::Terran_Medic)
            {
                medicUnits.push_back(unit);
            }
            else if (unitType == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode || unitType == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode)
            {
                tankUnits.push_back(unit);
            }   
			else if (unitType.isDetector() && !unitType.isBuilding())
			{
				detectorUnits.push_back(unit);
			}
			// select transport _units
			else if (unitType == BWAPI::UnitTypes::Protoss_Shuttle || unitType == BWAPI::UnitTypes::Terran_Dropship)
			{
				transportUnits.push_back(unit);
			}
			// select ranged _units
			else if ((unitType.groundWeapon().maxRange() > 32) || (unitType == BWAPI::UnitTypes::Protoss_Reaver) || (unitType == BWAPI::UnitTypes::Zerg_Scourge))
			{
				rangedUnits.push_back(unit);
			}
			// select melee _units
			else if (unitType.groundWeapon().maxRange() <= 32)
			{
				meleeUnits.push_back(unit);
			}
		}
	}

	_meleeManager.setUnits(meleeUnits);
	_rangedManager.setUnits(rangedUnits);
	_detectorManager.setUnits(detectorUnits);
	_transportManager.setUnits(transportUnits);
    _tankManager.setUnits(tankUnits);
    _medicManager.setUnits(medicUnits);
}

// calculates whether or not to regroup
bool Squad::needsToRegroup(shared_ptr<MapTools> map, int currentFrame)
{
    if (!Config::Micro::UseSparcraftSimulation)
    {
        return false;
    }

	// if we are not attacking, never regroup
	if (_units.empty() || (_order.getType() != SquadOrderTypes::Attack))
	{
		_regroupStatus = std::string("\x04 No combat units available");
		return false;
	}

	auto distanceFunction = [&map](const BWAPI::Position & src, const BWAPI::Position & dest)
	{
		return map->getGroundDistance(src, dest);
	};
	BWAPI::Unit unitClosest = unitClosestToEnemy(distanceFunction);

	if (!unitClosest)
	{
		_regroupStatus = std::string("\x04 No closest unit");
		return false;
	}

    // if none of our units are in attack range of any enemy units, don't retreat
    std::vector<UnitInfo> enemyCombatUnits;
    const auto & enemyUnitInfo = _unitInfo->getUnitInfoMap(Global::getEnemy());

    bool anyInRange = false;
    for (const auto & eui : enemyUnitInfo)
    {
        bool inRange = false;
        for (const auto & u : _units)
        {
            int range = UnitUtil::GetAttackRange(eui.second.type, u->getType());
            if (range + 128 >= eui.second.lastPosition.getDistance(u->getPosition()))
            {
                inRange = true;
                break;
            }
        }

        if (inRange)
        {
            anyInRange = true;
            break;
        }
    }

    if (!anyInRange)
    {
        _regroupStatus = std::string("\x04 No enemy units in attack range");
        return false;
    }

	// if we are DT rushing and we haven't lost a DT yet, no retreat!
	if (Config::Strategy::StrategyName == "Protoss_DTRush"
		&& (_opponentView->self()->deadUnitCount(BWAPI::UnitTypes::Protoss_Dark_Templar) == 0))
	{
		_regroupStatus = std::string("\x04 DARK TEMPLAR HOOOOO!");
		return false;
	}

	auto simulationCenter = unitClosest->getPosition();
	std::vector<BWAPI::Unit> ourCombatUnits;
	UnitUtil::getUnitsInRadius(
		_opponentView,
		ourCombatUnits,
		simulationCenter,
		Config::Micro::CombatRegroupRadius,
		true,
		false);

	std::vector<UnitInfo> enemyCombatUnitsForSimulation;
	for (auto& enemyPlayer : _opponentView->enemies())
	{
		_unitInfo->getNearbyForce(enemyCombatUnitsForSimulation, simulationCenter, enemyPlayer, Config::Micro::CombatRegroupRadius);
	}

	//do the SparCraft Simulation!
	CombatSimulation sim(_opponentView, _logger);
	sim.setCombatUnits(ourCombatUnits, enemyCombatUnitsForSimulation, simulationCenter, Config::Micro::CombatRegroupRadius, currentFrame);
    auto score = sim.simulateCombat();

	bool retreat = score < 0;
    int switchTime = 100;
    bool waiting = false;

    // we should not attack unless 5 seconds have passed since a retreat
    if (retreat != _lastRetreatSwitchVal)
    {
        if (!retreat && (currentFrame - _lastRetreatSwitch < switchTime))
        {
            waiting = true;
            retreat = _lastRetreatSwitchVal;
        }
        else
        {
            waiting = false;
            _lastRetreatSwitch = currentFrame;
            _lastRetreatSwitchVal = retreat;
        }
    }
	
	if (retreat)
	{
		_regroupStatus = std::string("\x04 Retreat - simulation predicts defeat");
	}
	else
	{
		_regroupStatus = std::string("\x04 Attack - simulation predicts success");
	}

	return retreat;
}

void Squad::setSquadOrder(const SquadOrder & so)
{
	_order = so;
}

bool Squad::containsUnit(BWAPI::Unit u) const
{
    return std::find(_units.begin(), _units.end(), u) != _units.end();
}

void Squad::clear(int currentFrame)
{
	auto handler = _onRemoveHandler;
    for (auto & unit : getUnits())
    {
		if (handler)
		{
			handler(unit, currentFrame);
		}
    }

    _units.clear();
}

bool Squad::unitNearEnemy(shared_ptr<MapTools> map, BWAPI::Unit unit)
{
	assert(unit);

	std::vector<BWAPI::Unit> enemyNear;

	UnitUtil::getUnitsInRadius(
		_opponentView,
		enemyNear,
		unit->getPosition(),
		400,
		false,
		true);

	return enemyNear.size() > 0;
}

BWAPI::Position Squad::calcCenter()
{
    if (_units.empty())
    {
        if (Config::Debug::DrawSquadInfo)
        {
            _logger->log("Squad::calcCenter() called on empty squad");
        }

        return BWAPI::Position(0,0);
    }

	BWAPI::Position accum(0,0);
	for (auto & unit : _units)
	{
		accum += unit->getPosition();
	}

	return BWAPI::Position(accum.x / _units.size(), accum.y / _units.size());
}

BWAPI::Position Squad::calcRegroupPosition()
{
	BWAPI::Position regroup(0,0);

	int minDist = 100000;

	for (auto & unit : _units)
	{
		if (!_nearEnemy[unit])
		{
			int dist = unit->getDistance(_order.getPosition());
			if (dist < minDist)
			{
				minDist = dist;
				regroup = unit->getPosition();
			}
		}
	}

	if (regroup == BWAPI::Position(0,0))
	{
		return BWAPI::Position(_opponentView->self()->getStartLocation());
	}
	else
	{
		return regroup;
	}
}

BWAPI::Unit Squad::unitClosestToEnemy(std::function<int(const BWAPI::Position & src, const BWAPI::Position & dest)> distance)
{
	BWAPI::Unit closest = nullptr;
	int closestDist = 100000;

	for (auto & unit : _units)
	{
		if (unit->getType() == BWAPI::UnitTypes::Protoss_Observer)
		{
			continue;
		}

		// the distance to the order position
		int dist = distance(unit->getPosition(), _order.getPosition());

		if (dist != -1 && (!closest || dist < closestDist))
		{
			closest = unit;
			closestDist = dist;
		}
	}

	if (!closest)
	{
		for (auto & unit : _units)
		{
			if (unit->getType() == BWAPI::UnitTypes::Protoss_Observer)
			{
				continue;
			}

			// the distance to the order position
			int dist = unit->getDistance(BWAPI::Position(Global::getEnemy()->getStartLocation()));

			if (dist != -1 && (!closest || dist < closestDist))
			{
				closest = unit;
				closestDist = dist;
			}
		}
	}

	return closest;
}

int Squad::squadUnitsNear(BWAPI::Position p)
{
	int numUnits = 0;

	for (auto & unit : _units)
	{
		if (unit->getDistance(p) < 600)
		{
			numUnits++;
		}
	}

	return numUnits;
}

const std::set<BWAPI::Unit> & Squad::getUnits() const	
{ 
	return _units; 
} 

const SquadOrder & Squad::getSquadOrder()	const			
{ 
	return _order; 
}

void UAlbertaBot::Squad::onUnitRemoved(UnitHandler handler)
{
	_onRemoveHandler = handler;
}

void Squad::addUnit(BWAPI::Unit u)
{
	_units.insert(u);
}

void Squad::removeUnit(BWAPI::Unit u)
{
    _units.erase(u);
}

const std::string & Squad::getName() const
{
    return _name;
}

const std::string& Squad::getRegroupStatus() const
{
	return _regroupStatus;
}

bool Squad::getNeedToRegroup() const
{
	return _needToRegroup;
}
bool Squad::isNearEnemy(const BWAPI::Unit& unit) const
{
	return _nearEnemy.at(unit);
}