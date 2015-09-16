#include "MeleeManager.h"

using namespace UAlbertaBot;

MeleeManager::MeleeManager() { }

void MeleeManager::executeMicro(const std::vector<BWAPI::UnitInterface *> & targets) 
{
	const std::vector<BWAPI::UnitInterface *> & meleeUnits = getUnits();

	// figure out targets
	std::vector<BWAPI::UnitInterface *> meleeUnitTargets;
	for (size_t i(0); i<targets.size(); i++) 
	{
		// conditions for targeting
		if (!(targets[i]->getType().isFlyer()) && 
			!(targets[i]->isLifted()) &&
			!(targets[i]->getType() == BWAPI::UnitTypes::Zerg_Larva) && 
			!(targets[i]->getType() == BWAPI::UnitTypes::Zerg_Egg) &&
			targets[i]->isVisible()) 
		{
			meleeUnitTargets.push_back(targets[i]);
		}
	}

	// for each meleeUnit
	for (BWAPI::UnitInterface* meleeUnit : meleeUnits)
	{
		// if the order is to attack or defend
		if (order.type == order.Attack || order.type == order.Defend) {

			// if there are targets
			if (!meleeUnitTargets.empty())
			{
				// find the best target for this meleeUnit
				BWAPI::UnitInterface* target = getTarget(meleeUnit, meleeUnitTargets);
				// attack it
				smartAttackUnit(meleeUnit, target);
			}
			// if there are no targets
			else
			{
				// if we're not near the order position
				if (meleeUnit->getDistance(order.position) > 100)
				{
					// move to it
					smartMove(meleeUnit, order.position);
				}
			}
		}

		if (Config::Debug::DrawUnitTargetInfo)
		{
			BWAPI::Broodwar->drawLineMap(meleeUnit->getPosition().x, meleeUnit->getPosition().y, 
			meleeUnit->getTargetPosition().x, meleeUnit->getTargetPosition().y, Config::Debug::ColorLineTarget);
		}
	}
}

// get a target for the meleeUnit to attack
BWAPI::UnitInterface* MeleeManager::getTarget(BWAPI::UnitInterface* meleeUnit, std::vector<BWAPI::UnitInterface *> & targets)
{
	int highPriority(0);
	int closestDist(100000);
	BWAPI::UnitInterface* closestTarget = NULL;

	// for each target possiblity
	for (BWAPI::UnitInterface* unit : targets)
	{
		int priority = getAttackPriority(unit);
		if (meleeUnit->getType() == BWAPI::UnitTypes::Protoss_Dark_Templar && unit->getType().isWorker())
		{
			priority = 11;
		}

		if (Config::Debug::DrawUnitTargetInfo) BWAPI::Broodwar->drawTextMap(unit->getPosition().x, unit->getPosition().y, "%d", priority);

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
int MeleeManager::getAttackPriority(BWAPI::UnitInterface* unit) 
{
	BWAPI::UnitType type = unit->getType();

	// highest priority is something that can attack us or aid in combat
	if (type == BWAPI::UnitTypes::Terran_Medic || 
		(type.groundWeapon() != BWAPI::WeaponTypes::None && !type.isWorker()) || 
		type ==  BWAPI::UnitTypes::Terran_Bunker ||
		type == BWAPI::UnitTypes::Protoss_High_Templar ||
		type == BWAPI::UnitTypes::Protoss_Reaver ||
		(type.isWorker() && unitNearChokepoint(unit))) 
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
	else if (type == BWAPI::UnitTypes::Protoss_Pylon || type == BWAPI::UnitTypes::Zerg_Spire)
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

BWAPI::UnitInterface* MeleeManager::closestMeleeUnit(BWAPI::UnitInterface* target, std::set<BWAPI::UnitInterface*> & meleeUnitsToAssign)
{
	double minDistance = 0;
	BWAPI::UnitInterface* closest = NULL;

	for (BWAPI::UnitInterface* meleeUnit : meleeUnitsToAssign)
	{
		double distance = meleeUnit->getDistance(target);
		if (!closest || distance < minDistance)
		{
			minDistance = distance;
			closest = meleeUnit;
		}
	}
	
	return closest;
}