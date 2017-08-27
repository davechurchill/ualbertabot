#include "CombatCommander.h"
#include "UnitUtil.h"
#include "Global.h"
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/generator_iterator.hpp>

using namespace UAlbertaBot;
using namespace AKBot;

using boost::adaptors::filtered;

const size_t IdlePriority = 0;
const size_t AttackPriority = 1;
const size_t BaseDefensePriority = 2;
const size_t ScoutDefensePriority = 3;
const size_t DropPriority = 4;

CombatCommander::CombatCommander(
	shared_ptr<BaseLocationManager> baseLocationManager,
	shared_ptr<AKBot::OpponentView> opponentView,
	shared_ptr<WorkerManager> workerManager,
	shared_ptr<UnitInfoManager> unitInfo,
	shared_ptr<MapTools> mapTools,
	shared_ptr<AKBot::Logger> logger)
    : _initialized(false)
	, _unitInfo(unitInfo)
	, _baseLocationManager(baseLocationManager)
	, _workerManager(workerManager)
	, _mapTools(mapTools)
	, _playerLocationProvider(baseLocationManager)
	, _squadData(_playerLocationProvider, opponentView, unitInfo, baseLocationManager, mapTools, logger)
	, _opponentView(opponentView)
{
	_squadData.onUnitRemoved([this](const BWAPI::Unit& unit, int currentFrame)
	{
		if (unit->getType().isWorker())
		{
			_workerManager->finishedWithWorker(unit, currentFrame);
		}
	});
}

void CombatCommander::initializeSquads()
{
	auto startPosition = BWAPI::Position(_opponentView->self()->getStartLocation());
    SquadOrder idleOrder(SquadOrderTypes::Idle, startPosition, 100, "Chill Out");
	_squadData.addSquad("Idle", idleOrder, IdlePriority);

    // the main attack squad that will pressure the enemy's closest base location
    SquadOrder mainAttackOrder(
		SquadOrderTypes::Attack,
		getMainAttackLocation(startPosition),
		800,
		"Attack Enemy Base");
	_squadData.addSquad("MainAttack", mainAttackOrder, AttackPriority);

    BWAPI::Position ourBasePosition = BWAPI::Position(_opponentView->self()->getStartLocation());

    // the scout defense squad will handle chasing the enemy worker scout
    SquadOrder enemyScoutDefense(SquadOrderTypes::Defend, ourBasePosition, 900, "Get the scout");
    _squadData.addSquad("ScoutDefense", enemyScoutDefense, ScoutDefensePriority);

    // add a drop squad if we are using a drop strategy
	SquadOrder zealotDrop(SquadOrderTypes::Drop, ourBasePosition, 900, "Wait for transport");
	_squadData.addSquad("Drop", zealotDrop, DropPriority);

    _initialized = true;
}

bool CombatCommander::isSquadUpdateFrame(int currentFrame) const
{
	return currentFrame % 10 == 0;
}

void CombatCommander::update(const std::vector<BWAPI::Unit> & combatUnits, int currentFrame)
{
    if (!_initialized)
    {
        initializeSquads();
    }

    _combatUnits = combatUnits;

	if (isSquadUpdateFrame(currentFrame))
	{
        updateIdleSquad();

		if (Config.Strategy.StrategyName == "Protoss_Drop")
		{
			updateDropSquads();
		}

        updateScoutDefenseSquad(currentFrame);
		updateDefenseSquads(currentFrame);
		updateAttackSquads();
	}

	_squadData.update(_mapTools, currentFrame);
}

void CombatCommander::updateIdleSquad()
{
    Squad & idleSquad = _squadData.getSquad("Idle");
    for (auto & unit : _combatUnits)
    {
        // if it hasn't been assigned to a squad yet, put it in the low priority idle squad
        if (_squadData.canAssignUnitToSquad(unit, idleSquad))
        {
            idleSquad.addUnit(unit);
        }
    }
}

void CombatCommander::updateAttackSquads()
{
    Squad & mainAttackSquad = _squadData.getSquad("MainAttack");

	auto hydralistCount = UnitUtil::GetAllUnitCount(BWAPI::UnitTypes::Zerg_Hydralisk);
    for (auto & unit : _combatUnits)
    {
        if (unit->getType() == BWAPI::UnitTypes::Zerg_Scourge
			&& hydralistCount < 30)
        {
            continue;
        }

        // get every unit of a lower priority and put it into the attack squad
        if (!unit->getType().isWorker() 
			&& (unit->getType() != BWAPI::UnitTypes::Zerg_Overlord) 
			&& _squadData.canAssignUnitToSquad(unit, mainAttackSquad))
        {
            _squadData.assignUnitToSquad(unit, mainAttackSquad);
        }
    }

    SquadOrder mainAttackOrder(SquadOrderTypes::Attack, getMainAttackLocation(mainAttackSquad.calcCenter()), 800, "Attack Enemy Base");
    mainAttackSquad.setSquadOrder(mainAttackOrder);
}

void CombatCommander::updateDropSquads()
{
    Squad & dropSquad = _squadData.getSquad("Drop");

    // figure out how many units the drop squad needs
    bool dropSquadHasTransport = false;
    int transportSpotsRemaining = 8;
    auto & dropUnits = dropSquad.getUnits();

    for (auto & unit : dropUnits)
    {
        if (unit->isFlying() && unit->getType().spaceProvided() > 0)
        {
            dropSquadHasTransport = true;
        }
        else
        {
            transportSpotsRemaining -= unit->getType().spaceRequired();
        }
    }

    // if there are still units to be added to the drop squad, do it
    if (transportSpotsRemaining > 0 || !dropSquadHasTransport)
    {
        // take our first amount of combat units that fill up a transport and add them to the drop squad
        for (auto & unit : _combatUnits)
        {
            // if this is a transport unit and we don't have one in the squad yet, add it
            if (!dropSquadHasTransport && (unit->getType().spaceProvided() > 0 && unit->isFlying()))
            {
                _squadData.assignUnitToSquad(unit, dropSquad);
                dropSquadHasTransport = true;
                continue;
            }

            if (unit->getType().spaceRequired() > transportSpotsRemaining)
            {
                continue;
            }

            // get every unit of a lower priority and put it into the attack squad
            if (!unit->getType().isWorker() && _squadData.canAssignUnitToSquad(unit, dropSquad))
            {
                _squadData.assignUnitToSquad(unit, dropSquad);
                transportSpotsRemaining -= unit->getType().spaceRequired();
            }
        }
    }
    // otherwise the drop squad is full, so execute the order
    else
    {
        SquadOrder dropOrder(SquadOrderTypes::Drop, getMainAttackLocation(dropSquad.calcCenter()), 800, "Attack Enemy Base");
        dropSquad.setSquadOrder(dropOrder);
    }
}

void CombatCommander::updateScoutDefenseSquad(int currentFrame) 
{
    if (_combatUnits.empty()) 
    { 
        return; 
    }

    // if the current squad has units in it then we can ignore this
    Squad & scoutDefenseSquad = _squadData.getSquad("ScoutDefense");
  
    // get the region that our base is located in
    const BaseLocation * myBaseLocation = _baseLocationManager->getPlayerStartingBaseLocation(_opponentView->self());
    if (myBaseLocation == nullptr)
    {
        return;
    }

    // get all of the enemy units in this region
	std::vector<BWAPI::Unit> enemyUnitsInRegion;
    for (auto & unit : UnitUtil::getEnemyUnits(_opponentView))
    {
        if (myBaseLocation->containsPosition(unit->getPosition()))
        {
            enemyUnitsInRegion.push_back(unit);
        }
    }

    // if there's an enemy worker in our region then assign someone to chase him
    bool assignScoutDefender = enemyUnitsInRegion.size() == 1 && (*enemyUnitsInRegion.begin())->getType().isWorker();

    // if our current squad is empty and we should assign a worker, do it
    if (scoutDefenseSquad.isEmpty() && assignScoutDefender)
    {
        // the enemy worker that is attacking us
        BWAPI::Unit enemyWorker = *enemyUnitsInRegion.begin();

        // get our worker unit that is mining that is closest to it
        BWAPI::Unit workerDefender = findClosestWorkerToTarget(_combatUnits, enemyWorker);

		if (enemyWorker && workerDefender)
		{
			// grab it from the worker manager and put it in the squad
            if (_squadData.canAssignUnitToSquad(workerDefender, scoutDefenseSquad))
            {
				_workerManager->setCombatWorker(workerDefender, currentFrame);
                _squadData.assignUnitToSquad(workerDefender, scoutDefenseSquad);
            }
		}
    }
    // if our squad is not empty and we shouldn't have a worker chasing then take him out of the squad
    else if (!scoutDefenseSquad.isEmpty() && !assignScoutDefender)
    {
        for (auto & unit : scoutDefenseSquad.getUnits())
        {
            unit->stop();
            if (unit->getType().isWorker())
            {
				_workerManager->finishedWithWorker(unit, currentFrame);
            }
        }

        scoutDefenseSquad.clear(currentFrame);
    }
}

void CombatCommander::updateDefenseSquads(int currentFrame)
{
	if (_combatUnits.empty()) 
    { 
        return; 
    }

	auto enemy = Global::getEnemy();
	if (enemy == nullptr)
	{
		return;
	}

    const BaseLocation * enemyBaseLocation = _baseLocationManager->getPlayerStartingBaseLocation(enemy);

	// for each of our occupied regions
	for (const BaseLocation * myBaseLocation : _baseLocationManager->getOccupiedBaseLocations(_opponentView->self()))
	{
        // don't defend inside the enemy region, this will end badly when we are stealing gas
        if (myBaseLocation == enemyBaseLocation)
        {
            continue;
        }

		BWAPI::Position basePosition = myBaseLocation->getPosition();

		// start off assuming all enemy units in region are just workers
		int numDefendersPerEnemyUnit = 2;

		// all of the enemy units in this region
		std::vector<BWAPI::Unit> enemyUnitsInRegion;
        for (auto & unit : UnitUtil::getEnemyUnits(_opponentView))
        {
            // if it's an overlord, don't worry about it for defense, we don't care what they see
            if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord)
            {
                continue;
            }

            if (myBaseLocation->containsPosition(unit->getPosition()))
            {
                enemyUnitsInRegion.push_back(unit);
            }
        }

        // we can ignore the first enemy worker in our region since we assume it is a scout
        for (auto & unit : enemyUnitsInRegion)
        {
            if (unit->getType().isWorker())
            {
                enemyUnitsInRegion.erase(std::remove(enemyUnitsInRegion.begin(), enemyUnitsInRegion.end(), unit), enemyUnitsInRegion.end());
                break;
            }
        }

        int numEnemyFlyingInRegion = std::count_if(enemyUnitsInRegion.begin(), enemyUnitsInRegion.end(), [](BWAPI::Unit u) { return u->isFlying(); });
        int numEnemyGroundInRegion = std::count_if(enemyUnitsInRegion.begin(), enemyUnitsInRegion.end(), [](BWAPI::Unit u) { return !u->isFlying(); });

        std::stringstream squadName;
        squadName << "Base Defense " << basePosition.x << " " << basePosition.y; 
        
        // if there's nothing in this region to worry about
        if (enemyUnitsInRegion.empty())
        {
            // if a defense squad for this region exists, remove it
            if (_squadData.squadExists(squadName.str()))
            {
                _squadData.getSquad(squadName.str()).clear(currentFrame);
            }
            
            // and return, nothing to defend here
            continue;
        }
        else 
        {
            // if we don't have a squad assigned to this region already, create one
            if (!_squadData.squadExists(squadName.str()))
            {
                SquadOrder defendRegion(SquadOrderTypes::Defend, basePosition, 32 * 25, "Defend Region!");
                _squadData.addSquad(squadName.str(), defendRegion, BaseDefensePriority);
            }
        }

        // assign units to the squad
        if (_squadData.squadExists(squadName.str()))
        {
            Squad & defenseSquad = _squadData.getSquad(squadName.str());

            // figure out how many units we need on defense
	        int flyingDefendersNeeded = numDefendersPerEnemyUnit * numEnemyFlyingInRegion;
	        int groundDefensersNeeded = numDefendersPerEnemyUnit * numEnemyGroundInRegion;

            updateDefenseSquadUnits(defenseSquad, flyingDefendersNeeded, groundDefensersNeeded, currentFrame);
        }
        else
        {
            UAB_ASSERT_WARNING(false, "Squad should have existed: %s", squadName.str().c_str());
        }
	}

    // for each of our defense squads, if there aren't any enemy units near the position, remove the squad
    std::set<std::string> uselessDefenseSquads;
    for (const auto & kv : _squadData.getSquads())
    {
        const Squad & squad = kv.second;
        const SquadOrder & order = squad.getSquadOrder();

        if (order.getType() != SquadOrderTypes::Defend)
        {
            continue;
        }

        bool enemyUnitInRange = false;
        for (auto & unit : UnitUtil::getEnemyUnits(_opponentView))
        {
            if (unit->getPosition().getDistance(order.getPosition()) < order.getRadius())
            {
                enemyUnitInRange = true;
                break;
            }
        }

        if (!enemyUnitInRange)
        {
            _squadData.getSquad(squad.getName()).clear(currentFrame);
        }
    }
}

void CombatCommander::updateDefenseSquadUnits(
	Squad & defenseSquad,
	const size_t & flyingDefendersNeeded,
	const size_t & groundDefendersNeeded,
	int currentFrame)
{
    auto & squadUnits = defenseSquad.getUnits();
    size_t flyingDefendersInSquad = std::count_if(squadUnits.begin(), squadUnits.end(), UnitUtil::CanAttackAir);
    size_t groundDefendersInSquad = std::count_if(squadUnits.begin(), squadUnits.end(), UnitUtil::CanAttackGround);

    // if there's nothing left to defend, clear the squad
    if (flyingDefendersNeeded == 0 && groundDefendersNeeded == 0)
    {
        defenseSquad.clear(currentFrame);
        return;
    }

    // add flying defenders if we still need them
    size_t flyingDefendersAdded = 0;
    while (flyingDefendersNeeded > flyingDefendersInSquad + flyingDefendersAdded)
    {
        BWAPI::Unit defenderToAdd = findClosestDefender(defenseSquad, defenseSquad.getSquadOrder().getPosition(), true, currentFrame);

        // if we find a valid flying defender, add it to the squad
        if (defenderToAdd)
        {
            _squadData.assignUnitToSquad(defenderToAdd, defenseSquad);
            ++flyingDefendersAdded;
        }
        // otherwise we'll never find another one so break out of this loop
        else
        {
            break;
        }
    }

    // add ground defenders if we still need them
    size_t groundDefendersAdded = 0;
    while (groundDefendersNeeded > groundDefendersInSquad + groundDefendersAdded)
    {
        BWAPI::Unit defenderToAdd = findClosestDefender(defenseSquad, defenseSquad.getSquadOrder().getPosition(), false, currentFrame);

        // if we find a valid ground defender add it
        if (defenderToAdd)
        {
            _squadData.assignUnitToSquad(defenderToAdd, defenseSquad);
            ++groundDefendersAdded;
        }
        // otherwise we'll never find another one so break out of this loop
        else
        {
            break;
        }
    }
}

BWAPI::Unit CombatCommander::findClosestDefender(const Squad & defenseSquad, BWAPI::Position pos, bool flyingDefender, int currentFrame) 
{
	BWAPI::Unit closestDefender = nullptr;
	double minDistance = std::numeric_limits<double>::max();

    int zerglingsInOurBase = numZerglingsInOurBase();
    bool zerglingRush = zerglingsInOurBase > 0 && currentFrame < 5000;

	for (auto & unit : _combatUnits) 
	{
		if ((flyingDefender && !UnitUtil::CanAttackAir(unit)) || (!flyingDefender && !UnitUtil::CanAttackGround(unit)))
        {
            continue;
        }

        if (!_squadData.canAssignUnitToSquad(unit, defenseSquad))
        {
            continue;
        }

        // add workers to the defense squad if we are being rushed very quickly
        if (!Config.Micro.WorkersDefendRush || (unit->getType().isWorker() && !zerglingRush && !beingBuildingRushed()))
        {
            continue;
        }

        double dist = unit->getDistance(pos);
        if (!closestDefender || (dist < minDistance))
        {
            closestDefender = unit;
            minDistance = dist;
        }
	}

	return closestDefender;
}

BWAPI::Position CombatCommander::getDefendLocation()
{
	return _baseLocationManager->getPlayerStartingBaseLocation(_opponentView->self())->getPosition();
}

BWAPI::Position CombatCommander::getMainAttackLocation(BWAPI::Position squadPosition)
{
    // First choice: Attack an enemy region if we can see units inside it
	BWAPI::Position enemyBasePosition;
	if (findEnemyBaseLocation(enemyBasePosition))
	{
		return enemyBasePosition;
	}

	// Second choice: Attack known enemy buildings
	BWAPI::Position enemyBuildingPosition;
	if (findEnemyBuilding(enemyBuildingPosition))
	{
		return enemyBuildingPosition;
	}

    // Third choice: Attack visible enemy units that aren't overlords
	BWAPI::Position unitPosition;
	if (findEnemyUnit(squadPosition, unitPosition))
	{
		return unitPosition;
	}

    // Fourth choice: We can't see anything so explore the map attacking along the way
    return _baseLocationManager->getLeastRecentlySeenPosition(_mapTools);
}

BWAPI::Unit CombatCommander::findClosestWorkerToTarget(std::vector<BWAPI::Unit> & unitsToAssign, BWAPI::Unit target)
{
    UAB_ASSERT(target != nullptr, "target was null");

    if (!target)
    {
        return nullptr;
    }

    BWAPI::Unit closestMineralWorker = nullptr;
    double closestDist = 100000;
    
    // for each of our workers
	for (auto & unit : unitsToAssign)
	{
        if (!unit->getType().isWorker())
        {
            continue;
        }

		// if it is a move worker
        if (_workerManager->isFree(unit))
		{
			double dist = unit->getDistance(target);

            if (!closestMineralWorker || dist < closestDist)
            {
                closestMineralWorker = unit;
                dist = closestDist;
            }
		}
	}

    return closestMineralWorker;
}

// when do we want to defend with our workers?
// this function can only be called if we have no fighters to defend with
int CombatCommander::defendWithWorkers()
{
	// our home nexus position
	BWAPI::Position homePosition = _baseLocationManager->getPlayerStartingBaseLocation(_opponentView->self())->getPosition();

	// enemy units near our workers
	int enemyUnitsNearWorkers = 0;

	// defense radius of nexus
	int defenseRadius = 300;

	// fill the set with the types of units we're concerned about
	for (auto & unit : UnitUtil::getEnemyUnits(_opponentView))
	{
		// if it's a zergling or a worker we want to defend
		if (unit->getType() == BWAPI::UnitTypes::Zerg_Zergling)
		{
			if (unit->getDistance(homePosition) < defenseRadius)
			{
				enemyUnitsNearWorkers++;
			}
		}
	}

	// if there are enemy units near our workers, we want to defend
	return enemyUnitsNearWorkers;
}

int CombatCommander::numZerglingsInOurBase()
{
    int concernRadius = 600;
    int zerglings = 0;
    BWAPI::Position ourBasePosition = BWAPI::Position(_opponentView->self()->getStartLocation());
    
    // check to see if the enemy has zerglings as the only attackers in our base
    for (auto & unit : UnitUtil::getEnemyUnits(_opponentView))
    {
        if (unit->getType() != BWAPI::UnitTypes::Zerg_Zergling)
        {
            continue;
        }

        if (unit->getDistance(ourBasePosition) < concernRadius)
        {
            zerglings++;
        }
    }

    return zerglings;
}

bool CombatCommander::beingBuildingRushed()
{
    BWAPI::Position myBasePosition(_opponentView->self()->getStartLocation());

    // check to see if the enemy has buildings near our base
    for (auto & unit : UnitUtil::getEnemyUnits(_opponentView))
    {
        if (unit->getType().isBuilding() && unit->getDistance(myBasePosition) < 1200)
        {
            return true;
        }
    }

    return false;
}

bool UAlbertaBot::CombatCommander::findEnemyBaseLocation(BWAPI::Position & basePosition)
{
	for (auto& enemyPlayer : _opponentView->enemies())
	{
		const BaseLocation * enemyBaseLocation = _baseLocationManager->getPlayerStartingBaseLocation(enemyPlayer);
		if (enemyBaseLocation)
		{
			BWAPI::Position enemyBasePosition = enemyBaseLocation->getPosition();

			// If the enemy base hasn't been seen yet, go there.
			if (!BWAPI::Broodwar->isExplored(BWAPI::TilePosition(enemyBasePosition)))
			{
				basePosition = enemyBasePosition;
				return true;
			}

			// get all known enemy units in the area
			std::vector<BWAPI::Unit> enemyUnitsInArea;
			UnitUtil::getUnitsInRadius(
				_opponentView,
				enemyUnitsInArea,
				enemyBasePosition,
				800,
				false,
				true);

			for (auto & unit : enemyUnitsInArea)
			{
				if (unit->getType() != BWAPI::UnitTypes::Zerg_Overlord)
				{
					// Enemy base is not empty: It's not only overlords in the enemy base area.
					basePosition = enemyBasePosition;
					return true;
				}
			}
		}
	}

	return false;
}

bool UAlbertaBot::CombatCommander::findEnemyBuilding(BWAPI::Position & buildingPosition)
{
	for (auto& enemyPlayer : _opponentView->enemies())
	{
		for (const auto & kv : _unitInfo->getUnitInfoMap(enemyPlayer))
		{
			const UnitInfo & ui = kv.second;

			if (ui.type.isBuilding() && ui.lastPosition != BWAPI::Positions::None)
			{
				buildingPosition = ui.lastPosition;
				return true;
			}
		}
	}

	return false;
}

bool UAlbertaBot::CombatCommander::findEnemyUnit(BWAPI::Position targetPosition, BWAPI::Position & unitPosition)
{
	auto validUnitTypes = [this](const BWAPI::Unit& unit) {
		return unit->getType() != BWAPI::UnitTypes::Zerg_Overlord;
	};
	auto visibleUnits = [this](const BWAPI::Unit& unit) {
		return unit->isVisible();
	};
	auto distanceComparator = [&targetPosition](BWAPI::Unit a, BWAPI::Unit b) -> bool {
		return targetPosition.getDistance(a->getPosition()) > targetPosition.getDistance(b->getPosition());
	};

	bool takeFirst = true;
	BWAPI::Unit lastUnit = nullptr;
	for (auto& unit : UnitUtil::getEnemyUnits(_opponentView))
	{
		if (!validUnitTypes(unit))
		{
			continue;
		}

		if (!UnitUtil::IsValidUnit(unit))
		{
			continue;
		}

		if (!visibleUnits(unit))
		{
			continue;
		}

		if (lastUnit == nullptr)
		{
			lastUnit = unit;
			unitPosition = unit->getPosition();
			// If we take first unit only, then stop search there.
			if (takeFirst) {
				break;
			}
		}
		else
		{
			if (distanceComparator(unit, lastUnit))
			{
				lastUnit = unit;
				unitPosition = unit->getPosition();
			}
		}
	}

	return lastUnit != nullptr;
}

const SquadData& CombatCommander::getSquadData() const
{
	return _squadData;
}