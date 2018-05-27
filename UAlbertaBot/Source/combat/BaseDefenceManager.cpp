#include "BaseDefenceManager.h"
#include "CombatPriorities.h"

using namespace UAlbertaBot;
using namespace AKBot;

constexpr const int EarlyGameLastFrame = 5000;

AKBot::BaseDefenceManager::BaseDefenceManager(
	UAlbertaBot::SquadData & squadData,
	const BaseLocation* baseToProtect,
	const std::vector<BWAPI::Unit> & combatUnits,
	UnitCollection& enemyUnits,
	const BotMicroConfiguration& microConfiguration)
	: _squadData(squadData)
	, _combatUnits(combatUnits)
	, _enemyUnits(enemyUnits)
	, _microConfiguration(microConfiguration)
	, _baseToProtect(baseToProtect)
{
}

void AKBot::BaseDefenceManager::update(int currentFrame)
{
	totalGroundUnitsNearBases = 0;
	totalFlyingUnitsNearBases = 0;
	totalGroundDefendersAdded = 0;
	totalFlyingDefendersAdded = 0;
	totalGroundDefendersNeeded = 0;
	totalFlyingDefendersNeeded = 0;

	// all of the enemy units in this region
	UnitCollection enemyUnitsInRegion = getEnemyUnitsInRegion();

	BWAPI::Position basePosition = _baseToProtect->getPosition();
	auto squadName = getBaseDefenceSquadName(basePosition);
	auto baseDefenceSquadExists = _squadData.squadExists(squadName);

	// if there's nothing in this region to worry about
	if (!isDefenceRequired(enemyUnitsInRegion, currentFrame))
	{
		// if a defense squad for this region exists, remove it
		if (baseDefenceSquadExists)
		{
			_squadData.getSquad(squadName).clear(currentFrame);
		}

		// and return, nothing to defend here
		return;
	}
	else
	{
		// if we don't have a squad assigned to this region already, create one
		if (!baseDefenceSquadExists)
		{
			SquadOrder defendRegion(SquadOrderTypes::Defend, basePosition, 32 * 25, "Defend Region!");
			_squadData.addSquad(squadName, defendRegion, BaseDefensePriority);
		}
	}

	// assign units to the squad
	if (baseDefenceSquadExists)
	{
		auto& defenseSquad = _squadData.getSquad(squadName);

		int numEnemyGroundInRegion = getGroundEnemiesInRegion(enemyUnitsInRegion);
		totalGroundUnitsNearBases += numEnemyGroundInRegion;

		int numEnemyFlyingInRegion = getFlyingEnemiesInRegion(enemyUnitsInRegion);
		totalFlyingUnitsNearBases += numEnemyFlyingInRegion;

		// figure out how many units we need on defense
		int flyingDefendersNeeded = getFlyingDefendersNeeded(enemyUnitsInRegion);
		int groundDefensersNeeded = getGroundDefendersNeeded(enemyUnitsInRegion);

		updateDefenseSquadUnits(basePosition, defenseSquad, flyingDefendersNeeded, groundDefensersNeeded, currentFrame);
	}
	else
	{
		UAB_ASSERT_WARNING(false, "Squad should have existed: %s", squadName.c_str());
	}
}

UnitCollection AKBot::BaseDefenceManager::getEnemyUnitsInRegion() const
{
	BWAPI::Position basePosition = _baseToProtect->getPosition();

	// all of the enemy units in this region
	UnitCollection enemyUnitsInRegion;
	for (auto & unit : _enemyUnits)
	{
		// if it's an overlord, don't worry about it for defense, we don't care what they see
		if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord)
		{
			continue;
		}

		if (_baseToProtect->containsPosition(unit->getPosition()))
		{
			enemyUnitsInRegion.push_back(unit);
		}
	}

	return enemyUnitsInRegion;
}

bool AKBot::BaseDefenceManager::isDefenceRequired(UnitCollection enemyUnitsInRegion, int currentFrame) const
{
	auto noEnemies = enemyUnitsInRegion.empty();
	if (noEnemies)
	{
		return false;
	}

	auto isEarlyGame = currentFrame < EarlyGameLastFrame;
	if (!isEarlyGame) {
		// in the middle and late games, require that avery unit near the base would be a threat.
		return true;
	}

	auto singleEnemy = enemyUnitsInRegion.size() == 1;
	if (singleEnemy) {
		// Assume first worker is just scout.
		if (enemyUnitsInRegion.at(0)->getType().isWorker()) {
			return false;
		}
	}

	return true;
}

int AKBot::BaseDefenceManager::getFlyingDefendersNeeded(UnitCollection enemyUnitsInRegion) const
{
	// start off assuming all enemy units in region are just workers
	int numDefendersPerEnemyUnit = 2;
	
	int numEnemyFlyingInRegion = getFlyingEnemiesInRegion(enemyUnitsInRegion);

	int flyingDefendersNeeded = numDefendersPerEnemyUnit * numEnemyFlyingInRegion;
	return flyingDefendersNeeded;
}

int AKBot::BaseDefenceManager::getGroundDefendersNeeded(UnitCollection enemyUnitsInRegion) const
{
	// start off assuming all enemy units in region are just workers
	int numDefendersPerEnemyUnit = 2;

	int numEnemyGroundInRegion = getGroundEnemiesInRegion(enemyUnitsInRegion);

	int groundDefensersNeeded = numDefendersPerEnemyUnit * numEnemyGroundInRegion;
	return groundDefensersNeeded;
}

int AKBot::BaseDefenceManager::getFlyingEnemiesInRegion(UnitCollection enemyUnitsInRegion) const
{
	int numEnemyFlyingInRegion = std::count_if(enemyUnitsInRegion.begin(), enemyUnitsInRegion.end(), UnitUtil::IsFlyingUnit);
	return numEnemyFlyingInRegion;
}

int AKBot::BaseDefenceManager::getGroundEnemiesInRegion(UnitCollection enemyUnitsInRegion) const
{
	int numEnemyGroundInRegion = std::count_if(enemyUnitsInRegion.begin(), enemyUnitsInRegion.end(), UnitUtil::IsGroundUnit);
	return numEnemyGroundInRegion;
}

void AKBot::BaseDefenceManager::updateDefenseSquadUnits(
	BWAPI::Position basePosition,
	Squad & defenseSquad,
	const size_t & flyingDefendersNeeded,
	const size_t & groundDefendersNeeded,
	int currentFrame)
{
	auto & squadUnits = defenseSquad.getUnits();
	size_t flyingDefendersInSquad = std::count_if(squadUnits.begin(), squadUnits.end(), UnitUtil::CanAttackAir);
	size_t groundDefendersInSquad = std::count_if(squadUnits.begin(), squadUnits.end(), UnitUtil::CanAttackGround);
	totalFlyingDefendersNeeded += flyingDefendersNeeded;
	totalGroundDefendersNeeded += groundDefendersNeeded;

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
		BWAPI::Unit defenderToAdd = findClosestDefender(basePosition, defenseSquad, defenseSquad.getSquadOrder().getPosition(), true, currentFrame);

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
	totalFlyingDefendersAdded += flyingDefendersAdded;

	// add ground defenders if we still need them
	size_t groundDefendersAdded = 0;
	while (groundDefendersNeeded > groundDefendersInSquad + groundDefendersAdded)
	{
		BWAPI::Unit defenderToAdd = findClosestDefender(basePosition, defenseSquad, defenseSquad.getSquadOrder().getPosition(), false, currentFrame);

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
	totalGroundDefendersAdded += groundDefendersAdded;
}

BWAPI::Unit AKBot::BaseDefenceManager::findClosestDefender(BWAPI::Position basePosition, const Squad & defenseSquad, BWAPI::Position pos, bool useFlyingUnits, int currentFrame)
{
	BWAPI::Unit closestDefender = nullptr;
	double minDistance = std::numeric_limits<double>::max();

	int concerningRadius = getBaseDefenceRadius();
	auto zerglingsOnBase = numUnitsInOurBase(BWAPI::UnitTypes::Zerg_Zergling, basePosition, concerningRadius);
	auto scvOnBase = numUnitsInOurBase(BWAPI::UnitTypes::Terran_SCV, basePosition, concerningRadius);
	auto isEarlyGame = currentFrame < EarlyGameLastFrame;
	int unitInOurBase = zerglingsOnBase
		+ (scvOnBase > 1 || !isEarlyGame ? scvOnBase : 0);
	bool earlyRush = unitInOurBase > 0 && isEarlyGame;
	bool buildingRushed = beingBuildingRushed(basePosition);

	for (auto & unit : _combatUnits)
	{
		if ((useFlyingUnits && !UnitUtil::CanAttackAir(unit)) || (!useFlyingUnits && !UnitUtil::CanAttackGround(unit)))
		{
			continue;
		}

		if (!_squadData.canAssignUnitToSquad(unit, defenseSquad))
		{
			continue;
		}

		// add workers to the defense squad if we are being rushed very quickly
		if (!_microConfiguration.WorkersDefendRush
			|| (unit->getType().isWorker() && !earlyRush && !buildingRushed))
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

bool AKBot::BaseDefenceManager::beingBuildingRushed(BWAPI::Position basePosition)
{
	// check to see if the enemy has buildings near our base
	for (auto & unit : _enemyUnits)
	{
		if (unit->getType().isBuilding() && unit->getDistance(basePosition) < 1200)
		{
			return true;
		}
	}

	return false;
}

int AKBot::BaseDefenceManager::numUnitsInOurBase(BWAPI::UnitType unitType, BWAPI::Position basePosition, int radius)
{
	int concernRadius = 600;
	int zerglings = 0;

	// check to see if the enemy has zerglings as the only attackers in our base
	for (auto & unit : _enemyUnits)
	{
		if (unit->getType() != unitType)
		{
			continue;
		}

		if (unit->getDistance(basePosition) < concernRadius)
		{
			zerglings++;
		}
	}

	return zerglings;
}