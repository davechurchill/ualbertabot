#include "LurkerManager.h"
#include "UnitUtil.h"

using namespace UAlbertaBot;

LurkerManager::LurkerManager()
{
}

void LurkerManager::executeMicro(const BWAPI::Unitset & targets)
{
	assignTargetsOld(targets);
}


void LurkerManager::assignTargetsOld(const BWAPI::Unitset & targets)
{
	const BWAPI::Unitset & LurkerUnits = getUnits();

	// figure out targets
	BWAPI::Unitset LurkerUnitTargets;
	std::copy_if(targets.begin(), targets.end(), std::inserter(LurkerUnitTargets, LurkerUnitTargets.end()), [](BWAPI::Unit u){ return u->isVisible() && !u->isFlying(); });

	for (auto & LurkerUnit : LurkerUnits)
	{
		// if the order is to attack or defend
		if (order.getType() == SquadOrderTypes::Attack || order.getType() == SquadOrderTypes::Defend)
		{
			// if there are targets
			if (!LurkerUnitTargets.empty())
			{
				// find the best target for this lurker
				BWAPI::Unit target = getTarget(LurkerUnit, LurkerUnitTargets);
				/*if (!target) {
					LurkerUnit->unburrow();
				}*/
				
				if (target && Config::Debug::DrawUnitTargetInfo)
				{
					BWAPI::Broodwar->drawLineMap(LurkerUnit->getPosition(), LurkerUnit->getTargetPosition(), BWAPI::Colors::Purple);
				}

				// burrow and attack it
				if (LurkerUnit->getDistance(target) < BWAPI::UnitTypes::Zerg_Lurker.groundWeapon().maxRange()) {
					LurkerUnit->burrow();
					Micro::SmartAttackUnit(LurkerUnit, target);
					
				}
				else {
						LurkerUnit->unburrow();
						Micro::SmartAttackMove(LurkerUnit, order.getPosition());					
				}
				
			}
			// if there are no targets
			else
			{
				LurkerUnit->unburrow();
				// if we're not near the order position
				if (LurkerUnit->getDistance(order.getPosition()) > 100)
				{
					// move to it
					Micro::SmartAttackMove(LurkerUnit, order.getPosition());
				}
			}
		}
	}
}

std::pair<BWAPI::Unit, BWAPI::Unit> LurkerManager::findClosestUnitPair(const BWAPI::Unitset & attackers, const BWAPI::Unitset & targets)
{
	std::pair<BWAPI::Unit, BWAPI::Unit> closestPair(nullptr, nullptr);
	double closestDistance = std::numeric_limits<double>::max();

	for (auto & attacker : attackers)
	{
		BWAPI::Unit target = getTarget(attacker, targets);
		double dist = attacker->getDistance(attacker);

		if (!closestPair.first || (dist < closestDistance))
		{
			closestPair.first = attacker;
			closestPair.second = target;
			closestDistance = dist;
		}
	}

	return closestPair;
}

// get a target for the lurker to attack
BWAPI::Unit LurkerManager::getTarget(BWAPI::Unit LurkerUnit, const BWAPI::Unitset & targets)
{
	int bestPriorityDistance = 1000000;
	int bestPriority = 0;

	double bestLTD = 0;

	int highPriority = 0;
	double closestDist = std::numeric_limits<double>::infinity();
	BWAPI::Unit closestTarget = nullptr;

	for (const auto & target : targets)
	{
		double distance = LurkerUnit->getDistance(target);
		double LTD = UnitUtil::CalculateLTD(target, LurkerUnit);
		int priority = getAttackPriority(LurkerUnit, target);
		bool targetIsThreat = LTD > 0;

		if (!closestTarget || (priority > highPriority) || (priority == highPriority && distance < closestDist))
		{
			closestDist = distance;
			highPriority = priority;
			closestTarget = target;
		}
	}

	return closestTarget;
}

// get the attack priority of a type in relation to a zergling
int LurkerManager::getAttackPriority(BWAPI::Unit LurkerUnit, BWAPI::Unit target)
{
	BWAPI::UnitType LurkerType = LurkerUnit->getType();
	BWAPI::UnitType targetType = target->getType();

	// ignore all flying units

	bool isThreat = LurkerType.isFlyer() ? targetType.airWeapon() != BWAPI::WeaponTypes::None : targetType.groundWeapon() != BWAPI::WeaponTypes::None;

	if (target->getType().isWorker())
	{
		isThreat = false;
	}

	if (target->getType() == BWAPI::UnitTypes::Zerg_Lurker)
	{
		return 110;
	}

	if (target->getType() == BWAPI::UnitTypes::Zerg_Larva || target->getType() == BWAPI::UnitTypes::Zerg_Egg)
	{
		return 0;
	}

	// if the target is building something near our base something is fishy
	BWAPI::Position ourBasePosition = BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation());
	if (target->getType().isWorker() && (target->isConstructing() || target->isRepairing()) && target->getDistance(ourBasePosition) < 1200)
	{
		return 100;
	}

	if (target->getType().isBuilding() && (target->isCompleted() || target->isBeingConstructed()) && target->getDistance(ourBasePosition) < 1200)
	{
		return 90;
	}

	// highest priority is something that can attack us or aid in combat
	if (targetType == BWAPI::UnitTypes::Terran_Bunker || isThreat)
	{
		return 11;
	}
	// next priority is worker
	else if (targetType.isWorker())
	{
		if (LurkerUnit->getType() == BWAPI::UnitTypes::Terran_Vulture)
		{
			return 11;
		}

		return 11;
	}
	// next is special buildings
	else if (targetType == BWAPI::UnitTypes::Zerg_Spawning_Pool)
	{
		return 5;
	}
	// next is special buildings
	else if (targetType == BWAPI::UnitTypes::Protoss_Pylon)
	{
		return 5;
	}
	// next is buildings that cost gas
	else if (targetType.gasPrice() > 0)
	{
		return 4;
	}
	else if (targetType.mineralPrice() > 0)
	{
		return 3;
	}
	// then everything else
	else
	{
		return 1;
	}
}

BWAPI::Unit LurkerManager::closestLurkerUnit(BWAPI::Unit target, std::set<BWAPI::Unit> & LurkerUnitsToAssign)
{
	double minDistance = 0;
	BWAPI::Unit closest = nullptr;

	for (auto & LurkerUnit : LurkerUnitsToAssign)
	{
		double distance = LurkerUnit->getDistance(target);
		if (!closest || distance < minDistance)
		{
			minDistance = distance;
			closest = LurkerUnit;
		}
	}

	return closest;
}