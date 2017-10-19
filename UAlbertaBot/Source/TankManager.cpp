#include "TankManager.h"
#include "UnitUtil.h"
#include <BWAPI/Game.h>
#include <BWAPI/Player.h>
#include <BWAPI/TechType.h>
#include "Micro.h"

using namespace UAlbertaBot;

TankManager::TankManager(
	shared_ptr<AKBot::OpponentView> opponentView,
	shared_ptr<BaseLocationManager> bases,
	const BotMicroConfiguration& microConfiguration)
	: MicroManager(opponentView, bases)
	, _microConfiguration(microConfiguration)
{
}

void TankManager::executeMicro(const std::vector<BWAPI::Unit> & targets, int currentFrame)
{
	const std::vector<BWAPI::Unit> & tanks = getUnits();

	// figure out targets
	std::vector<BWAPI::Unit> tankTargets;
	std::copy_if(targets.begin(), targets.end(), std::inserter(tankTargets, tankTargets.end()),
		[](BWAPI::Unit u) { return u->isVisible() && !u->isFlying(); });

	int siegeTankRange = BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode.groundWeapon().maxRange() - 32;
	bool haveSiege = opponentView->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode);

	// for each zealot
	for (auto & tank : tanks)
	{
		// train sub units such as scarabs or interceptors
		//trainSubUnits(rangedUnit);

		// TODO:: fix this
		bool tankNearChokepoint = false;

		// if the order is to attack or defend
		if (order.getType() == SquadOrderTypes::Attack || order.getType() == SquadOrderTypes::Defend)
		{
			// if there are targets
			if (!tankTargets.empty())
			{
				// find the best target for this zealot
				BWAPI::Unit target = getTarget(tank, tankTargets);

				// Code below now moved to CombatCommanderDebug
				// I don't know how target variable and rangedUnit->getTargetPosition() related right now
				/*if (target && _microConfiguration.DrawUnitTargetInfo)
				{
					canvas.drawLineMap(tank->getPosition(), tank->getTargetPosition(), BWAPI::Colors::Purple);
				}*/

				// if we are within siege range, siege up
				if (tank->getDistance(target) < siegeTankRange && tank->canSiege() && !tankNearChokepoint)
				{
					tank->siege();
				}
				// otherwise unsiege and move in
				else if ((!target || tank->getDistance(target) > siegeTankRange) && tank->canUnsiege())
				{
					tank->unsiege();
				}

				// if we're in siege mode just attack the target
				if (tank->isSieged())
				{
					Micro::SmartAttackUnit(tank, target, currentFrame);
				}
				// if we're not in siege mode kite the target
				else
				{
					Micro::SmartKiteTarget(tank, target, currentFrame, _microConfiguration.KiteLongerRangedUnits);
				}
			}
			// if there are no targets
			else
			{
				// if we're not near the order position
				if (tank->getDistance(order.getPosition()) > 100)
				{
					if (tank->canUnsiege())
					{
						tank->unsiege();
					}
					else
					{
						// move to it
						Micro::SmartAttackMove(tank, order.getPosition(), currentFrame);
					}
				}
			}
		}
	}
}

// get a target for the zealot to attack
BWAPI::Unit TankManager::getTarget(BWAPI::Unit tank, const std::vector<BWAPI::Unit> & targets)
{
	int bestPriorityDistance = 1000000;
	int bestPriority = 0;

	double bestLTD = 0;

	BWAPI::Unit bestTargetThreatInRange = nullptr;
	double bestTargetThreatInRangeLTD = 0;

	int highPriority = 0;
	double closestDist = std::numeric_limits<double>::infinity();
	BWAPI::Unit closestTarget = nullptr;

	int siegeTankRange = BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode.groundWeapon().maxRange() - 32;
	std::vector<BWAPI::Unit> targetsInSiegeRange;
	for (auto & target : targets)
	{
		if (target->getDistance(tank) < siegeTankRange && UnitUtil::CanAttack(tank, target))
		{
			targetsInSiegeRange.push_back(target);
		}
	}

	const std::vector<BWAPI::Unit> & newTargets = targetsInSiegeRange.empty() ? targets : targetsInSiegeRange;

	// check first for units that are in range of our attack that can cause damage
	// choose the highest priority one from them at the lowest health
	for (const auto & target : newTargets)
	{
		if (!UnitUtil::CanAttack(tank, target))
		{
			continue;
		}

		double distance = tank->getDistance(target);
		double LTD = UnitUtil::CalculateLTD(target, tank);
		int priority = getAttackPriority(tank, target);
		bool targetIsThreat = LTD > 0;
		BWAPI::Broodwar->drawTextMap(target->getPosition(), "%d", priority);

		if (!closestTarget || (priority > highPriority) || (priority == highPriority && distance < closestDist))
		{
			closestDist = distance;
			highPriority = priority;
			closestTarget = target;
		}
	}

	if (bestTargetThreatInRange)
	{
		return bestTargetThreatInRange;
	}

	return closestTarget;
}

// get the attack priority of a type in relation to a zergling
int TankManager::getAttackPriority(BWAPI::Unit rangedUnit, BWAPI::Unit target)
{
	BWAPI::UnitType rangedType = rangedUnit->getType();
	BWAPI::UnitType targetType = target->getType();

	bool isThreat = rangedType.isFlyer() ? targetType.airWeapon() != BWAPI::WeaponTypes::None : targetType.groundWeapon() != BWAPI::WeaponTypes::None;

	if (target->getType().isWorker())
	{
		isThreat = false;
	}

	if (target->getType() == BWAPI::UnitTypes::Zerg_Larva || target->getType() == BWAPI::UnitTypes::Zerg_Egg)
	{
		return 0;
	}

	// if the target is building something near our base something is fishy
	BWAPI::Position ourBasePosition = BWAPI::Position(opponentView->self()->getStartLocation());
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
		return 9;
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

BWAPI::Unit TankManager::closestrangedUnit(BWAPI::Unit target, std::set<BWAPI::Unit> & rangedUnitsToAssign)
{
	double minDistance = 0;
	BWAPI::Unit closest = nullptr;

	for (auto & rangedUnit : rangedUnitsToAssign)
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