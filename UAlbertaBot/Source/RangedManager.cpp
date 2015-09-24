#include "RangedManager.h"

using namespace UAlbertaBot;

RangedManager::RangedManager() 
{ 
}

void RangedManager::executeMicro(const BWAPI::Unitset & targets) 
{
	const BWAPI::Unitset & rangedUnits = getUnits();

	// figure out targets
	BWAPI::Unitset rangedUnitTargets;
	for (BWAPI::UnitInterface * target : targets) 
	{
		// conditions for targeting
		if (target->isVisible()) 
		{
			rangedUnitTargets.insert(target);
            BWAPI::Broodwar->drawCircleMap(target->getPosition(), 8, BWAPI::Colors::Red);
		}
	}
    
	// for each zealot
	for (BWAPI::UnitInterface* rangedUnit : rangedUnits)
	{
		// train sub units such as scarabs or interceptors
		//trainSubUnits(rangedUnit);

		// if the order is to attack or defend
		if (order.getType() == SquadOrderTypes::Attack || order.getType() == SquadOrderTypes::Defend) 
        {
			// if there are targets
			if (!rangedUnitTargets.empty())
			{
				// find the best target for this zealot
				BWAPI::UnitInterface* target = getTarget(rangedUnit, rangedUnitTargets);
                
                if (target && Config::Debug::DrawUnitTargetInfo) 
	            {
		            BWAPI::Broodwar->drawLineMap(rangedUnit->getPosition(), rangedUnit->getTargetPosition(), BWAPI::Colors::Purple);
	            }

				// attack it
                if (Config::Micro::KiteWithRangedUnits)
                {
                    if (rangedUnit->getType() == BWAPI::UnitTypes::Zerg_Mutalisk)
                    {
				        Micro::MutaDanceTarget(rangedUnit, target);
                    }
                    else
                    {
                        Micro::SmartKiteTarget(rangedUnit, target);
                    }
                }
                else
                {
                    Micro::SmartAttackUnit(rangedUnit, target);
                }
			}
			// if there are no targets
			else
			{
				// if we're not near the order position
				if (rangedUnit->getDistance(order.getPosition()) > 100)
				{
					// move to it
					Micro::SmartAttackMove(rangedUnit, order.getPosition());
				}
			}
		}

		
	}
}

// get a target for the zealot to attack
BWAPI::UnitInterface* RangedManager::getTarget(BWAPI::UnitInterface* rangedUnit, const BWAPI::Unitset & targets)
{
	int range(rangedUnit->getType().groundWeapon().maxRange());

    int bestPriorityDistance = 1000000;
    int bestPriority = 0;
    int bestPriorityHP = 100000;

	BWAPI::UnitInterface * bestTarget = NULL;
   

    if (rangedUnit->isFlying())
    {
	    for (BWAPI::UnitInterface* unit : targets)
	    {
		    int priority = getAttackPriority(rangedUnit, unit);
		    int distance = rangedUnit->getDistance(unit);
            int hp = unit->getHitPoints() + unit->getShields();

            bool newBest = !bestTarget;
            if (priority > bestPriority)
            {
                newBest = true;
            }
            else if (priority == bestPriority)
            {
                if (hp < bestPriorityHP)
                {
                    newBest = true;
                }
                else if (hp == bestPriority && distance < bestPriorityDistance)
                {
                    newBest = true;
                }
            }

            if (newBest)
            {
                bestTarget = unit;
                bestPriorityDistance = distance;
                bestPriority = priority;
                bestPriorityHP = hp;
            }
	    }
    }
    else
    {
        for (BWAPI::UnitInterface* unit : targets)
	    {
		    int priority = getAttackPriority(rangedUnit, unit);
		    int distance = rangedUnit->getDistance(unit);
            int hp = unit->getHitPoints() + unit->getShields();

            if (!bestTarget || (priority > bestPriority) || (priority == bestPriority && distance < bestPriorityDistance))
            {
                bestTarget = unit;
                bestPriorityDistance = distance;
                bestPriority = priority;
                bestPriorityHP = hp;
            }
	    }
    }

	// if there is a highest priority unit in range, attack it first
	return bestTarget;
}

	// get the attack priority of a type in relation to a zergling
int RangedManager::getAttackPriority(BWAPI::UnitInterface* rangedUnit, BWAPI::UnitInterface* target) 
{
	BWAPI::UnitType rangedType = rangedUnit->getType();
	BWAPI::UnitType targetType = target->getType();

	bool canAttackUs = rangedType.isFlyer() ? targetType.airWeapon() != BWAPI::WeaponTypes::None : targetType.groundWeapon() != BWAPI::WeaponTypes::None;

	// highest priority is something that can attack us or aid in combat
    if (targetType ==  BWAPI::UnitTypes::Terran_Bunker || canAttackUs)
    {
        return 11;
    }
    else if (targetType == BWAPI::UnitTypes::Terran_Medic ||  
		targetType ==  BWAPI::UnitTypes::Terran_Bunker ||
		targetType == BWAPI::UnitTypes::Protoss_High_Templar ||
		targetType == BWAPI::UnitTypes::Protoss_Reaver ||
		(targetType.isWorker() && unitNearChokepoint(target))) 
	{
		return 10;
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