#include "RangedManager.h"
#include "UnitUtil.h"
#include <BWAPI/Game.h>
#include <BWAPI/Player.h>
#include "Micro.h"

using namespace UAlbertaBot;
using AKBot::RangeUnitObservation;

RangedManager::RangedManager(
	shared_ptr<AKBot::OpponentView> opponentView,
	shared_ptr<BaseLocationManager> bases,
	const BotMicroConfiguration& microConfiguration)
	: MicroManager(opponentView, bases, microConfiguration)
	, _rangeUnitTargets()
	, _observations()
{ 
}

void RangedManager::executeMicro(const std::vector<BWAPI::Unit> & targets, int currentFrame)
{
	// if the order is to attack or defend
	if (order.getType() == SquadOrderTypes::Attack || order.getType() == SquadOrderTypes::Defend)
	{
		const std::vector<BWAPI::Unit> & rangedUnits = getUnits();

		assignTargets(rangedUnits, targets);
		executePlan(currentFrame);
	}
}

bool isRangedTarget(BWAPI::Unit target)
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

	if (target->getType() == BWAPI::UnitTypes::Unknown)
	{
		return false;
	}

	return true;
}

void RangedManager::populateRangeTargets(const std::vector<BWAPI::Unit> & targets)
{
	_rangeUnitTargets.clear();
	std::copy_if(
		targets.begin(),
		targets.end(),
		std::back_inserter(_rangeUnitTargets),
		isRangedTarget);
}

void RangedManager::collectObservations(const std::vector<BWAPI::Unit> & rangedUnits, const std::vector<BWAPI::Unit> & targets)
{
	_observations.clear();

	for (auto & rangedUnit : rangedUnits)
	{
		// train sub units such as scarabs or interceptors
		//trainSubUnits(rangedUnit);

		RangeUnitObservation observation;
		observation.rangedTargets = _rangeUnitTargets.size();
		observation.orderDistance = rangedUnit->getDistance(order.getPosition());
		_observations[rangedUnit] = observation;
	}
}

void RangedManager::generatePlan(const std::vector<BWAPI::Unit> & rangedUnits)
{
	for (auto & rangedUnit : rangedUnits)
	{
		RangeUnitObservation observation = _observations[rangedUnit];
		observation.shouldMove = false;
		observation.shouldAttack = false;
		observation.shouldMutaDance = false;
		observation.shouldKiteTarget = false;

		// if there are targets
		if (observation.rangedTargets > 0)
		{
			// find the best target for this zealot
			BWAPI::Unit target = getTarget(rangedUnit, _rangeUnitTargets);

			// attack it
			if (configuration().KiteWithRangedUnits)
			{
				if (rangedUnit->getType() == BWAPI::UnitTypes::Zerg_Mutalisk || rangedUnit->getType() == BWAPI::UnitTypes::Terran_Vulture)
				{
					observation.shouldMutaDance = true;
					observation.targetUnit = target;
				}
				else
				{
					observation.shouldKiteTarget = true;
					observation.targetUnit = target;
				}
			}
			else
			{
				observation.shouldAttack = true;
				observation.targetUnit = target;
			}
		}
		// if there are no targets
		else
		{
			// if we're not near the order position
			if (observation.orderDistance > configuration().MoveTargetThreshold)
			{
				// move to it
				observation.shouldMove = true;
				observation.targetPosition = order.getPosition();
			}
		}

		_observations[rangedUnit] = observation;
	}
}

void RangedManager::executePlan(int currentFrame)
{
	for (auto & rangedUnitPlanEntry : _observations)
	{
		RangeUnitObservation observation = rangedUnitPlanEntry.second;
		auto& rangedUnit = rangedUnitPlanEntry.first;
		if (observation.shouldMove)
		{
			Micro::SmartAttackMove(rangedUnit, observation.targetPosition, currentFrame);
		}

		if (observation.shouldAttack)
		{
			Micro::SmartAttackUnit(rangedUnit, observation.targetUnit, currentFrame);
		}

		if (observation.shouldMutaDance)
		{
			Micro::MutaDanceTarget(rangedUnit, observation.targetUnit);
		}

		if (observation.shouldKiteTarget)
		{
			Micro::SmartKiteTarget(opponentView->self(), rangedUnit, observation.targetUnit, currentFrame, configuration().KiteLongerRangedUnits);
		}
	}
}

void RangedManager::assignTargets(const std::vector<BWAPI::Unit> & rangedUnits, const std::vector<BWAPI::Unit> & targets)
{
	// figure out targets
	populateRangeTargets(targets);
	collectObservations(rangedUnits, targets);
	generatePlan(rangedUnits);
}

// get a target for the zealot to attack
BWAPI::Unit RangedManager::getTarget(BWAPI::Unit rangedUnit, const std::vector<BWAPI::Unit> & targets)
{
	int bestPriorityDistance = 1000000;
    int bestPriority = 0;
    
    double bestLTD = 0;

    int highPriority = 0;
	double closestDist = std::numeric_limits<double>::infinity();
	BWAPI::Unit closestTarget = nullptr;

    for (const auto & target : targets)
    {
        double distance         = rangedUnit->getDistance(target);
        double LTD              = UnitUtil::CalculateLTD(target, rangedUnit);
        int priority            = getAttackPriority(rangedUnit, target);
        bool targetIsThreat     = LTD > 0;
        
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
int RangedManager::getAttackPriority(BWAPI::Unit rangedUnit, BWAPI::Unit target) 
{
	BWAPI::UnitType rangedType = rangedUnit->getType();
	BWAPI::UnitType targetType = target->getType();

    if (rangedUnit->getType() == BWAPI::UnitTypes::Zerg_Scourge)
    {
        if (target->getType() == BWAPI::UnitTypes::Protoss_Carrier)
        {
            return 100;
        }

        if (target->getType() == BWAPI::UnitTypes::Protoss_Corsair)
        {
            return 90;
        }
    }

	bool isThreat = rangedType.isFlyer()
		? targetType.airWeapon() != BWAPI::WeaponTypes::None 
		: targetType.groundWeapon() != BWAPI::WeaponTypes::None;

    if (target->getType().isWorker())
    {
        isThreat = false;
    }

    if (target->getType() == BWAPI::UnitTypes::Zerg_Larva || target->getType() == BWAPI::UnitTypes::Zerg_Egg)
    {
        return 0;
    }

    if (rangedUnit->isFlying() && target->getType() == BWAPI::UnitTypes::Protoss_Carrier)
    {
        return 101;
    }

    // if the target is building something near our base something is fishy
    BWAPI::Position ourBasePosition = BWAPI::Position(opponentView->self()->getStartLocation());
    if (target->getType().isWorker()
		&& (target->isConstructing() || target->isRepairing()) && target->getDistance(ourBasePosition) < 1200)
    {
        return 100;
    }

    if (target->getType().isBuilding()
		&& (target->isCompleted() || target->isBeingConstructed())
		&& target->getDistance(ourBasePosition) < 1200)
    {
        return 90;
    }
    
	// highest priority is something that can attack us or aid in combat
    if (targetType ==  BWAPI::UnitTypes::Terran_Bunker || isThreat)
    {
        return 11;
    }
	// next priority is worker
	else if (targetType.isWorker()) 
	{
        if (rangedUnit->getType() == BWAPI::UnitTypes::Terran_Vulture)
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

const std::map<BWAPI::Unit, AKBot::RangeUnitObservation>& RangedManager::getObservations() const
{
	return _observations;
}

const std::vector<BWAPI::Unit>& RangedManager::getTargets() const
{
	return _rangeUnitTargets;
}
