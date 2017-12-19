#include "MeleeManager.h"
#include "UnitUtil.h"
#include <BWAPI/Game.h>
#include <BWAPI/Player.h>
#include "Micro.h"

using namespace UAlbertaBot;
using AKBot::MeeleUnitObservation;

MeleeManager::MeleeManager(
	shared_ptr<AKBot::OpponentView> opponentView,
	shared_ptr<BaseLocationManager> bases,
	const BotMicroConfiguration& microConfiguration)
	: MicroManager(opponentView, bases, microConfiguration)
	, _meleeUnitTargets()
	, _observations()
{
}

void MeleeManager::executeMicro(const std::vector<BWAPI::Unit> & targets, int currentFrame)
{
	if (order.getType() == SquadOrderTypes::Attack || order.getType() == SquadOrderTypes::Defend)
	{
		const std::vector<BWAPI::Unit> & meleeUnits = getUnits();
		assignTargets(meleeUnits, targets);
		executePlan(currentFrame);
	}
}

bool isMeeleTarget(BWAPI::Unit target)
{
	if (!target->isVisible())
	{
		return false;
	}

	if (target->getHitPoints() <= 0
		&& target->getShields() <= 0)
	{
		return false;
	}

	if (!(target->getType().isFlyer()) &&
		!(target->isLifted()) &&
		!(target->getType() == BWAPI::UnitTypes::Zerg_Larva) &&
		!(target->getType() == BWAPI::UnitTypes::Zerg_Egg))
	{
		return true;
	}

	return false;
}

void MeleeManager::populateMeleeTargets(const std::vector<BWAPI::Unit> & targets)
{
	_meleeUnitTargets.clear();
	std::copy_if(targets.begin(), targets.end(), std::back_inserter(_meleeUnitTargets), isMeeleTarget);
}

void MeleeManager::collectObservations(const std::vector<BWAPI::Unit> & meleeUnits, const std::vector<BWAPI::Unit> & targets)
{
	_observations.clear();

	// Capture basic observations about world.
	for (auto & meleeUnit : meleeUnits)
	{
		MeeleUnitObservation observation;
		observation.shouldRetreat = meleeUnitShouldRetreat(meleeUnit, targets);
		observation.meleeTargets = _meleeUnitTargets.size();
		observation.orderDistance = meleeUnit->getDistance(order.getPosition());
		_observations[meleeUnit] = observation;
	}
}

void MeleeManager::generatePlan(const std::vector<BWAPI::Unit> & meleeUnits)
{
	// Do simple reasoning about current status.
	for (auto & meleeUnit : meleeUnits)
	{
		MeeleUnitObservation observation = _observations[meleeUnit];
		observation.shouldAttack = false;
		observation.shouldMove = false;

		// run away if we meet the retreat critereon
		if (observation.shouldRetreat)
		{
			BWAPI::Position fleeTo(opponentView->self()->getStartLocation());
			observation.targetPosition = fleeTo;
			observation.shouldMove = true;
		}
		// if there are targets
		else if (observation.meleeTargets > 0)
		{
			// find the best target for this meleeUnit
			BWAPI::Unit target = getTarget(meleeUnit, _meleeUnitTargets);
			observation.targetUnit = target;
			observation.shouldAttack = true;
		}
		// if there are no targets
		else
		{
			// if we're not near the order position
			if (observation.orderDistance > configuration().MoveTargetThreshold)
			{
				// move to it
				observation.targetPosition = order.getPosition();
				observation.shouldMove = true;
			}
		}

		_observations[meleeUnit] = observation;
	}
}

void MeleeManager::executePlan(int currentFrame)
{
	// Execute plan of actions.
	for (auto & meleeUnitPlanEntry : _observations)
	{
		MeeleUnitObservation observation = meleeUnitPlanEntry.second;
		if (observation.shouldMove)
		{
			Micro::SmartMove(meleeUnitPlanEntry.first, observation.targetPosition, currentFrame);
		}

		if (observation.shouldAttack)
		{
			Micro::SmartAttackUnit(meleeUnitPlanEntry.first, observation.targetUnit, currentFrame);
		}
	}
}

void MeleeManager::assignTargets(const std::vector<BWAPI::Unit> & meleeUnits, const std::vector<BWAPI::Unit> & targets)
{
	// figure out targets
	populateMeleeTargets(targets);
	collectObservations(meleeUnits, targets);
	generatePlan(meleeUnits);
}

std::pair<BWAPI::Unit, BWAPI::Unit> MeleeManager::findClosestUnitPair(const std::vector<BWAPI::Unit> & attackers, const std::vector<BWAPI::Unit> & targets)
{
    std::pair<BWAPI::Unit, BWAPI::Unit> closestPair(nullptr, nullptr);
    double closestDistance = std::numeric_limits<double>::max();

    for (auto & attacker : attackers)
    {
        BWAPI::Unit target = getTarget(attacker, targets);
        double dist = attacker->getDistance(target);

        if (!closestPair.first || (dist < closestDistance))
        {
            closestPair.first = attacker;
            closestPair.second = target;
            closestDistance = dist;
        }
    }

    return closestPair;
}

// get a target for the meleeUnit to attack
BWAPI::Unit MeleeManager::getTarget(BWAPI::Unit meleeUnit, const std::vector<BWAPI::Unit> & targets)
{
	int highPriority = 0;
	double closestDist = std::numeric_limits<double>::infinity();
	BWAPI::Unit closestTarget = nullptr;

	// for each target possiblity
	for (auto & unit : targets)
	{
		int priority = getAttackPriority(meleeUnit, unit);
		int distance = meleeUnit->getDistance(unit);

		// if it's a higher priority, or it's closer, set it
		if (!closestTarget || (priority > highPriority) || (priority == highPriority && distance < closestDist))
		{
			closestDist = distance;
			highPriority = priority;
			closestTarget = unit;
		}
	}

	return closestTarget;
}

	// get the attack priority of a type in relation to a zergling
int MeleeManager::getAttackPriority(BWAPI::Unit attacker, BWAPI::Unit unit) 
{
	BWAPI::UnitType type = unit->getType();

    if (attacker->getType() == BWAPI::UnitTypes::Protoss_Dark_Templar 
        && unit->getType() == BWAPI::UnitTypes::Terran_Missile_Turret
        && (opponentView->self()->deadUnitCount(BWAPI::UnitTypes::Protoss_Dark_Templar) == 0))
    {
        return 13;
    }

	if (attacker->getType() == BWAPI::UnitTypes::Protoss_Dark_Templar && type.isWorker())
	{
		// This is probably due to fact, that killing of worker by Dark Templar is very fast
		// So this is simplification of more generic rule:
		// Kill as much as possible within specific time frame
		// Also this priority does not include how safe this attack could be.

		// After some research in Liquipedia - http://wiki.teamliquid.net/starcraft/Dark_Templar
		// I found out that if unit die in single hit, it will not trigger - your forces is under attack
		// warning, which probably less interesting for the bot plays.
		return 12;
	}

	// highest priority is something that can attack us or aid in combat
    if (type == BWAPI::UnitTypes::Terran_Bunker)
    {
        return 11;
    }
    else if (type == BWAPI::UnitTypes::Terran_Medic || 
		(type.groundWeapon() != BWAPI::WeaponTypes::None && !type.isWorker()) || 
		type == BWAPI::UnitTypes::Protoss_High_Templar ||
		type == BWAPI::UnitTypes::Protoss_Reaver) 
	{
		return 10;
	} 
	// next priority is worker
	else if (type.isWorker()) 
	{
		return 9;
	}
    // next is special buildings
	else if (type == BWAPI::UnitTypes::Zerg_Spawning_Pool)
	{
		return 5;
	}
	// next is special buildings
	else if (type == BWAPI::UnitTypes::Protoss_Pylon)
	{
		return 5;
	}
	// next is buildings that cost gas
	else if (type.gasPrice() > 0)
	{
		return 4;
	}
	else if (type.mineralPrice() > 0)
	{
		return 3;
	}
	// then everything else
	else
	{
		return 1;
	}
}

bool MeleeManager::meleeUnitShouldRetreat(BWAPI::Unit meleeUnit, const std::vector<BWAPI::Unit> & targets)
{
    // terran don't regen so it doesn't make any sense to retreat
    if (meleeUnit->getType().getRace() == BWAPI::Races::Terran)
    {
        return false;
    }

    // we don't want to retreat the melee unit if its shields or hit points are above the threshold set in the config file
    // set those values to zero if you never want the unit to retreat from combat individually
    if (meleeUnit->getShields() >= configuration().RetreatMeleeUnitShields || meleeUnit->getHitPoints() >= configuration().RetreatMeleeUnitHP)
    {
        return false;
    }

    // if there is a ranged enemy unit within attack range of this melee unit then we shouldn't bother retreating since it could fire and kill it anyway
    for (auto & unit : targets)
    {
        int groundWeaponRange = unit->getType().groundWeapon().maxRange();
        if (groundWeaponRange >= 64 && unit->getDistance(meleeUnit) < groundWeaponRange)
        {
            return false;
        }
    }

    return true;
}

const std::map<BWAPI::Unit, AKBot::MeeleUnitObservation>& MeleeManager::getObservations() const
{
	return _observations;
}

const std::vector<BWAPI::Unit>& MeleeManager::getTargets() const
{
	return _meleeUnitTargets;
}