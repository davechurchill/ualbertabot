#include "DetectorManager.h"

using namespace UAlbertaBot;

DetectorManager::DetectorManager() : unitClosestToEnemy(NULL) { }


void DetectorManager::executeMicro(const std::vector<BWAPI::UnitInterface *> & targets) 
{
	const std::vector<BWAPI::UnitInterface *> & detectorUnits = getUnits();
	/*
	check if each unit's radius contains enemy/biological/energy>0/shield>0/notirradiated depending on race
		calculate best value through evaluation function of number of units and perhaps hp/stats
		cast emp/irradiate on best enemy unit
	otherwise matrix most hp attacking unit mech > bio
	eraser: cast irradiate on each mech unit and move around #unfeasible?

	TODO: fix movement, research irradiate 4 zerg and emp 4 toss, matrix the best unit,perhaps add more logic in attackpriority, scout maybe
	*/
	if (detectorUnits.empty())
	{
		return;
	}
	if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
	{

		std::vector<BWAPI::UnitInterface *> detectorUnitTargets;
		for (size_t i(0); i < targets.size(); i++)
		{
			// conditions for targeting
			if (targets[i]->isVisible())
			{
				detectorUnitTargets.push_back(targets[i]);
			}
		}
		for (BWAPI::UnitInterface* detectorUnit : detectorUnits)
		{
			// if there are targets
			if (!detectorUnitTargets.empty())
			{
				// find the best target for this sci ves
				BWAPI::UnitInterface* target = getTarget(detectorUnit, detectorUnitTargets);
				if (target == nullptr) continue;
				// attack it with magic
				kiteTarget(detectorUnit, target);
			}
			// if there are no targets
			else
			{
				// if we're not near the order position
				break;
			}


			if (Config::Debug::DrawUnitTargetInfo)
			{
				BWAPI::Broodwar->drawLineMap(detectorUnit->getPosition().x, detectorUnit->getPosition().y,
					detectorUnit->getTargetPosition().x, detectorUnit->getTargetPosition().y, Config::Debug::ColorLineTarget);
			}
		}
	}


	cloakedUnitMap.clear();
	std::vector<BWAPI::UnitInterface *> cloakedUnits;

	// figure out targets
	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->enemy()->getUnits())
	{
		// conditions for targeting
		if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker ||
			unit->getType() == BWAPI::UnitTypes::Protoss_Dark_Templar ||
			unit->getType() == BWAPI::UnitTypes::Terran_Wraith) 
		{
			cloakedUnits.push_back(unit);
			cloakedUnitMap[unit] = false;
		}
	}

	bool detectorUnitInBattle = false;
	//TODO maybe, current sci ves do not scout, if there are multiple science vessels, send one with least hp/energy to scout map
	//		maybe mini scout - continously scout enemy base until air defenses are up

	for (BWAPI::UnitInterface* detectorUnit : detectorUnits)
	{
		// if we need to regroup, move the detectorUnit to that location
		if (!detectorUnitInBattle && unitClosestToEnemy && unitClosestToEnemy->getPosition().isValid())
		{
			smartMove(detectorUnit, unitClosestToEnemy->getPosition());
			if (detectorUnit->getType() != BWAPI::UnitTypes::Terran_Science_Vessel) detectorUnitInBattle = true;
		}
		// otherwise there is no battle or no closest to enemy so we don't want our detectorUnit to die
		// send him to scout around the map
		else
		{
			if (detectorUnit->getType() == BWAPI::UnitTypes::Terran_Science_Vessel) return;
			BWAPI::Position explorePosition = MapGrid::Instance().getLeastExplored();
			smartMove(detectorUnit, explorePosition);
		}
	}
}


BWAPI::UnitInterface* DetectorManager::closestCloakedUnit(const std::vector<BWAPI::UnitInterface *> & cloakedUnits, BWAPI::UnitInterface* detectorUnit)
{
	BWAPI::UnitInterface* closestCloaked = NULL;
	double closestDist = 100000;

	for (BWAPI::UnitInterface* unit : cloakedUnits)
	{
		// if we haven't already assigned an detectorUnit to this cloaked unit
		if (!cloakedUnitMap[unit])
		{
			double dist = unit->getDistance(detectorUnit);

			if (!closestCloaked || (dist < closestDist))
			{
				closestCloaked = unit;
				closestDist = dist;
			}
		}
	}

	return closestCloaked;
}
void DetectorManager::kiteTarget(BWAPI::UnitInterface* detectorUnit, BWAPI::UnitInterface* target)
{

	double range(9*32); //irradiate

	// determine whether the target can be kited
	if (range <= target->getType().groundWeapon().maxRange())
	{
		// if we can't kite it, there's no point
		spellAttackUnit(detectorUnit, target);
		return;
	}

	double		minDist(64);
	bool		kite(true);
	double		dist(detectorUnit->getDistance(target));
	double		speed(detectorUnit->getType().topSpeed());

	double	timeToEnter = std::max(0.0, (dist - range) / speed);
	if (dist >= minDist)
	{
		kite = false;
	}

	if (target->getType().isBuilding() && target->getType() != BWAPI::UnitTypes::Terran_Bunker)
	{
		kite = false;
	}

	if (detectorUnit->isSelected())
	{
		BWAPI::Broodwar->drawCircleMap(detectorUnit->getPosition().x, detectorUnit->getPosition().y,
			(int)range, BWAPI::Colors::Cyan);
	}

	// if we can't shoot, run away
	if (kite)
	{
		BWAPI::Position fleePosition(detectorUnit->getPosition() - target->getPosition() + detectorUnit->getPosition());

		BWAPI::Broodwar->drawLineMap(detectorUnit->getPosition().x, detectorUnit->getPosition().y,
			fleePosition.x, fleePosition.y, BWAPI::Colors::Cyan);

		smartMove(detectorUnit, fleePosition);
	}
	// otherwise shoot
	else
	{
		spellAttackUnit(detectorUnit, target);
	}
}

// get a target for the detector to attack
BWAPI::UnitInterface* DetectorManager::getTarget(BWAPI::UnitInterface* detectorUnit, std::vector<BWAPI::UnitInterface *> & targets)
{
	int range(9*32); //Irradiate range is 9, emp is 8

	int highestInRangePriority(0);
	int highestNotInRangePriority(0);
	int highestInRangeHitPoints(10000);
	int lowestNotInRangeDistance(10000);

	BWAPI::UnitInterface* inRangeTarget = NULL;
	BWAPI::UnitInterface* notInRangeTarget = NULL;

	for (BWAPI::UnitInterface* unit : targets)
	{
		int priority = getAttackPriority(detectorUnit, unit);
		int distance = detectorUnit->getDistance(unit);

		// if the unit is in range, update the target with the highest hp
		if (detectorUnit->getDistance(unit) <= range)
		{
			if (priority > highestInRangePriority ||
				(priority == highestInRangePriority && unit->getHitPoints() > highestInRangeHitPoints))
			{
				highestInRangeHitPoints = unit->getHitPoints();
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
	// No valid targets
	if (highestInRangePriority == 1 && highestNotInRangePriority == 1) return nullptr;
	// if there is a highest priority unit in range, attack it first
	return (highestInRangePriority >= highestNotInRangePriority) ? inRangeTarget : notInRangeTarget;
}

// get the attack priority of a type in relation to a high priority units
int DetectorManager::getAttackPriority(BWAPI::UnitInterface* detectorUnit, BWAPI::UnitInterface* target)
{
	BWAPI::UnitType detectorUnitType = detectorUnit->getType();
	BWAPI::UnitType targetType = target->getType();

	//bool canAttackUs = detectorUnitType.isFlyer() ? targetType.airWeapon() != BWAPI::WeaponTypes::None : targetType.groundWeapon() != BWAPI::WeaponTypes::None;


	// LOGIC GOES HERE-------------------------
	// highest priority is something that we can irradiate or aid in combat with energy
	if ((targetType == BWAPI::UnitTypes::Terran_Medic||
		targetType == BWAPI::UnitTypes::Terran_Ghost) ||
		target->getEnergy() > 90 || 
		target->getUnitsInRadius(64, !BWAPI::Filter::IsIrradiated
			&& BWAPI::Filter::IsOrganic
			&& BWAPI::Filter::IsVisible
			&& BWAPI::Filter::IsEnemy).size() >= 2)
	{
		return 3;
	}
	// next priority is worker
	else if (targetType.isWorker() && targetType != BWAPI::UnitTypes::Protoss_Probe)
	{
		return 2;
	}
	// then everything else
	else
	{
		return 1;
	}
}

BWAPI::UnitInterface* DetectorManager::closestdetectorUnit(BWAPI::UnitInterface* target, std::set<BWAPI::UnitInterface*> & detectorUnitsToAssign)
{
	double minDistance = 0;
	BWAPI::UnitInterface* closest = NULL;

	for (BWAPI::UnitInterface* detectorUnit : detectorUnitsToAssign)
	{
		double distance = detectorUnit->getDistance(target);
		if (!closest || distance < minDistance)
		{
			minDistance = distance;
			closest = detectorUnit;
		}
	}
	return closest;
}

/* Could possibly expand to only use specific spells on specific enemies eg. emp on archon, but Imma assume that
	we'll only research EMP if we're up against protoss, irradiate against zerg, and nothing for terran
*/
void DetectorManager::spellAttackUnit(BWAPI::UnitInterface* detector, BWAPI::UnitInterface* target) {
	if (detector->canUseTech(BWAPI::TechTypes::EMP_Shockwave, NULL)) detector->useTech(BWAPI::TechTypes::EMP_Shockwave, target);
	else if (detector->canUseTech(BWAPI::TechTypes::Irradiate, NULL)) detector->useTech(BWAPI::TechTypes::Irradiate, target);
	else { 
		//TODO Protect strongest unit while attacking or doing something important eg && BWAPI::Filter::IsMechanical
		auto ally = detector->getClosestUnit(
			BWAPI::Filter::IsVisible &&
			BWAPI::Filter::IsAlly &&
			!BWAPI::Filter::IsDetector && 
			!BWAPI::Filter::IsDefenseMatrixed, 10 * 32);//max range is 10
		if (ally == nullptr) return;
		detector->useTech(BWAPI::TechTypes::Defensive_Matrix, ally);
	}
}


