#include "RangedManager.h"

using namespace UAlbertaBot;

RangedManager::RangedManager() { }

void RangedManager::executeMicro(const std::vector<BWAPI::UnitInterface *> & targets) 
{
	const std::vector<BWAPI::UnitInterface *> & rangedUnits = getUnits();

	// figure out targets
	std::vector<BWAPI::UnitInterface *> rangedUnitTargets;
	for (size_t i(0); i<targets.size(); i++) 
	{
		// conditions for targeting
		if (targets[i]->isVisible()) 
		{
			rangedUnitTargets.push_back(targets[i]);
		}
	}

	// for each zealot
	for (BWAPI::UnitInterface* rangedUnit : rangedUnits)
	{
		// train sub units such as scarabs or interceptors
		//trainSubUnits(rangedUnit);

		// if the order is to attack or defend
		if (order.type == order.Attack || order.type == order.Defend) {

			// if there are targets
			if (!rangedUnitTargets.empty())
			{
				// find the best target for this zealot
				BWAPI::UnitInterface* target = getTarget(rangedUnit, rangedUnitTargets);

				// attack it
				kiteTarget(rangedUnit, target);
			}
			// if there are no targets
			else
			{
				// if we're not near the order position
				if (rangedUnit->getDistance(order.position) > 100)
				{
					// move to it
					smartAttackMove(rangedUnit, order.position);
				}
			}
		}

		if (Config::Debug::DrawUnitTargetInfo) 
		{
			BWAPI::Broodwar->drawLineMap(rangedUnit->getPosition().x, rangedUnit->getPosition().y, 
				rangedUnit->getTargetPosition().x, rangedUnit->getTargetPosition().y, Config::Debug::ColorLineTarget);
		}
	}
}

void RangedManager::kiteTarget(BWAPI::UnitInterface* rangedUnit, BWAPI::UnitInterface* target)
{
	
	double range(rangedUnit->getType().groundWeapon().maxRange());
	if (rangedUnit->getType() == BWAPI::UnitTypes::Protoss_Dragoon && BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Singularity_Charge))
	{
		range = 6*32;
	}

	// determine whether the target can be kited
	if (range <= target->getType().groundWeapon().maxRange())
	{
		// if we can't kite it, there's no point
		smartAttackUnit(rangedUnit, target);
		return;
	}

	double		minDist(64);
	bool		kite(true);
	double		dist(rangedUnit->getDistance(target));
	double		speed(rangedUnit->getType().topSpeed());

	double	timeToEnter = std::max(0.0,(dist - range) / speed);
	if ((timeToEnter >= rangedUnit->getGroundWeaponCooldown()) && (dist >= minDist))
	{
		kite = false;
	}

	if (target->getType().isBuilding() && target->getType() != BWAPI::UnitTypes::Terran_Bunker)
	{
		kite = false;
	}

	if (rangedUnit->isSelected())
	{
		BWAPI::Broodwar->drawCircleMap(rangedUnit->getPosition().x, rangedUnit->getPosition().y, 
			(int)range, BWAPI::Colors::Cyan);
	}

	// if we can't shoot, run away
	if (kite)
	{
		BWAPI::Position fleePosition(rangedUnit->getPosition() - target->getPosition() + rangedUnit->getPosition());

		BWAPI::Broodwar->drawLineMap(rangedUnit->getPosition().x, rangedUnit->getPosition().y, 
			fleePosition.x, fleePosition.y, BWAPI::Colors::Cyan);

		smartMove(rangedUnit, fleePosition);
	}
	// otherwise shoot
	else
	{
		smartAttackUnit(rangedUnit, target);
	}
}

// get a target for the zealot to attack
BWAPI::UnitInterface* RangedManager::getTarget(BWAPI::UnitInterface* rangedUnit, std::vector<BWAPI::UnitInterface *> & targets)
{
	int range(rangedUnit->getType().groundWeapon().maxRange());

	int highestInRangePriority(0);
	int highestNotInRangePriority(0);
	int lowestInRangeHitPoints(10000);
	int lowestNotInRangeDistance(10000);

	BWAPI::UnitInterface* inRangeTarget = NULL;
	BWAPI::UnitInterface* notInRangeTarget = NULL;

	for (BWAPI::UnitInterface* unit : targets)
	{
		int priority = getAttackPriority(rangedUnit, unit);
		int distance = rangedUnit->getDistance(unit);

		// if the unit is in range, update the target with the lowest hp
		if (rangedUnit->getDistance(unit) <= range)
		{
			if (priority > highestInRangePriority ||
				(priority == highestInRangePriority && unit->getHitPoints() < lowestInRangeHitPoints))
			{
				lowestInRangeHitPoints = unit->getHitPoints();
				highestInRangePriority = priority;
				inRangeTarget = unit;
			}
		}
		// otherwise it isn't in range so see if it's closest
		else
		{
			if (priority > highestNotInRangePriority ||
				(priority == highestNotInRangePriority && distance < lowestNotInRangeDistance))
			{
				lowestNotInRangeDistance = distance;
				highestNotInRangePriority = priority;
				notInRangeTarget = unit;
			}
		}
	}

	// if there is a highest priority unit in range, attack it first
	return (highestInRangePriority >= highestNotInRangePriority) ? inRangeTarget : notInRangeTarget;
}

	// get the attack priority of a type in relation to a zergling
int RangedManager::getAttackPriority(BWAPI::UnitInterface* rangedUnit, BWAPI::UnitInterface* target) 
{
	BWAPI::UnitType rangedUnitType = rangedUnit->getType();
	BWAPI::UnitType targetType = target->getType();

	bool canAttackUs = rangedUnitType.isFlyer() ? targetType.airWeapon() != BWAPI::WeaponTypes::None : targetType.groundWeapon() != BWAPI::WeaponTypes::None;

	

	// highest priority is something that can attack us or aid in combat
	if (targetType == BWAPI::UnitTypes::Terran_Medic || canAttackUs ||
		targetType ==  BWAPI::UnitTypes::Terran_Bunker) 
	{
		return 3;
	} 
	// next priority is worker
	else if (targetType.isWorker()) 
	{
		return 2;
	} 
	// then everything else
	else 
	{
		return 1;
	}
}

BWAPI::UnitInterface* RangedManager::closestrangedUnit(BWAPI::UnitInterface* target, std::set<BWAPI::UnitInterface*> & rangedUnitsToAssign)
{
	double minDistance = 0;
	BWAPI::UnitInterface* closest = NULL;

	for (BWAPI::UnitInterface* rangedUnit : rangedUnitsToAssign)
	{
		double distance = rangedUnit->getDistance(target);
		if (!closest || distance < minDistance)
		{
			minDistance = distance;
			closest = rangedUnit;
		}
	}
	
	return closest;
}