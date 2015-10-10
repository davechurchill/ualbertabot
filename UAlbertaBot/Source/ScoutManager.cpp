#include "ScoutManager.h"

using namespace UAlbertaBot;

ScoutManager::ScoutManager() : workerScout(NULL), numWorkerScouts(0), scoutUnderAttack(false)
{
}

void ScoutManager::update(const std::set<BWAPI::UnitInterface*> & scoutUnits)
{
	if (scoutUnits.size() == 1)
	{
		BWAPI::UnitInterface* scoutUnit = *scoutUnits.begin();

		if (scoutUnit->getType().isWorker())
		{
			if (scoutUnit != workerScout)
			{
				numWorkerScouts++;
				workerScout = scoutUnit;
			}
		}
	}

	moveScouts();
}

void ScoutManager::moveScouts()
{
	if (!workerScout || !workerScout->exists() || !workerScout->getPosition().isValid() || !(workerScout->getHitPoints() > 0))
	{
		return;
	}

	// get the enemy base location, if we have one
	BWTA::BaseLocation * enemyBaseLocation = InformationManager::Instance().getMainBaseLocation(BWAPI::Broodwar->enemy());

	// determine the region that the enemy is in
	BWTA::Region * enemyRegion = enemyBaseLocation ? enemyBaseLocation->getRegion() : NULL;

	// determine the region the scout is in
	BWAPI::TilePosition scoutTile(workerScout->getPosition());
	BWTA::Region * scoutRegion = scoutTile.isValid() ? BWTA::getRegion(scoutTile) : NULL;

	// we only care if the scout is under attack within the enemy region
	// this ignores if their scout worker attacks it on the way to their base
	if (workerScout->isUnderAttack() && (scoutRegion == enemyRegion))
	{
		scoutUnderAttack = true;
	}

	if (!workerScout->isUnderAttack() && !enemyWorkerInRadius())
	{
		scoutUnderAttack = false;
	}

	// if we know where the enemy region is and where our scout is
	if (enemyRegion && scoutRegion)
	{
		// if the scout is in the enemy region
		if (scoutRegion == enemyRegion)
		{
			std::vector<GroundThreat> groundThreats;
			fillGroundThreats(groundThreats, workerScout->getPosition());

			// get the closest enemy worker
			BWAPI::UnitInterface* closestWorker = closestEnemyWorker();

			// if the worker scout is not under attack
			if (!scoutUnderAttack)
			{
				// if there is a worker nearby, harass it
				if (closestWorker && (workerScout->getDistance(closestWorker) < 800))
				{
					smartAttack(workerScout, closestWorker);
				}
				// otherwise keep moving to the enemy region
				else
				{
					// move to the enemy region
					smartMove(workerScout, enemyBaseLocation->getPosition());
					BWAPI::Broodwar->drawLineMap(workerScout->getPosition().x, workerScout->getPosition().y, 
						enemyBaseLocation->getPosition().x, enemyBaseLocation->getPosition().y,
						BWAPI::Colors::Yellow);
				}
				
			}
			// if the worker scout is under attack
			else
			{
				/*BWAPI::Position fleeTo = calcFleePosition(groundThreats, NULL);
				if (Config::Debug::DrawUnitTargetInfo) BWAPI::Broodwar->drawCircleMap(fleeTo.x, fleeTo.y, 10, BWAPI::Colors::Red);

				for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->getUnitsInRadius(fleeTo, 10))
				{
					if (Config::Debug::DrawUnitTargetInfo) BWAPI::Broodwar->drawCircleMap(unit->getPosition().x, unit->getPosition().y, 5, BWAPI::Colors::Cyan, true);
				}*/

                BWAPI::Position fleeTo = BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation());

				smartMove(workerScout, fleeTo);
			}
		}
		// if the scout is not in the enemy region
		else if (scoutUnderAttack)
		{
			smartMove(workerScout, BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()));
		}
		else
		{
			// move to the enemy region
			smartMove(workerScout, enemyBaseLocation->getPosition());	
		}
		
	}

	// for each start location in the level
	if (!enemyRegion)
	{
		for (BWTA::BaseLocation * startLocation : BWTA::getStartLocations()) 
		{
			// if we haven't explored it yet
			if (!BWAPI::Broodwar->isExplored(startLocation->getTilePosition())) 
			{
				// assign a zergling to go scout it
				smartMove(workerScout, BWAPI::Position(startLocation->getTilePosition()));			
				return;
			}
		}
	}
}

BWAPI::Position ScoutManager::calcFleePosition(const std::vector<GroundThreat> & threats, BWAPI::UnitInterface* target) 
{
	// calculate the standard flee vector
	double2 fleeVector = getFleeVector(threats);

	// vector to the target, if one exists
	double2 targetVector(0,0);

	// normalise the target vector
	if (target) 
	{
		targetVector = target->getPosition() - workerScout->getPosition();
		targetVector.normalise();
	}

	int mult = 1;

	if (workerScout->getID() % 2) 
	{
		mult = -1;
	}

	// rotate the flee vector by 30 degrees, this will allow us to circle around and come back at a target
	//fleeVector.rotate(mult*30);
	double2 newFleeVector;

	int r = 0;
	int sign = 1;
	int iterations = 0;
		
	// keep rotating the vector until the new position is able to be walked on
	while (r <= 360) 
	{
		// rotate the flee vector
		fleeVector.rotate(mult*r);

		// re-normalize it
		fleeVector.normalise();

		// new vector should semi point back at the target
		newFleeVector = fleeVector * 2 + targetVector;

		// the position we will attempt to go to
		BWAPI::Position test(workerScout->getPosition() + newFleeVector * 24);

	    if (Config::Debug::DrawUnitTargetInfo)
        {
            BWAPI::Broodwar->drawLineMap(workerScout->getPosition().x, workerScout->getPosition().y, test.x, test.y, BWAPI::Colors::Cyan);
        }
		

		// if the position is able to be walked on, break out of the loop
		if (isValidFleePosition(test))
		{
			break;
		}

		r = r + sign * (r + 10);
		sign = sign * -1;

		if (++iterations > 36)
		{
			break;
		}
	}

	// go to the calculated 'good' position
	BWAPI::Position fleeTo(workerScout->getPosition() + newFleeVector * 24);
	
	
	if (Config::Debug::DrawUnitTargetInfo)
    {
        BWAPI::Broodwar->drawLineMap(workerScout->getPosition().x, workerScout->getPosition().y, fleeTo.x, fleeTo.y, BWAPI::Colors::Orange);
    }
	

	return fleeTo;
}

double2 ScoutManager::getFleeVector(const std::vector<GroundThreat> & threats)
{
	double2 fleeVector(0,0);

	for (const GroundThreat & threat : threats)
	{
		// Get direction from enemy to mutalisk
		const double2 direction(workerScout->getPosition() - threat.unit->getPosition());

		// Divide direction by square distance, weighting closer enemies higher
		//  Dividing once normalises the vector
		//  Dividing a second time reduces the effect of far away units
		const double distanceSq(direction.lenSq());
		if(distanceSq > 0)
		{
			// Enemy influence is direction to flee from enemy weighted by danger posed by enemy
			const double2 enemyInfluence( (direction / distanceSq) * threat.weight );

			fleeVector = fleeVector + enemyInfluence;
		}
	}

	if(fleeVector.lenSq() == 0)
	{
		// Flee towards our base
		fleeVector = double2(1,0);	
	}

	fleeVector.normalise();

	BWAPI::Position p1(workerScout->getPosition());
	BWAPI::Position p2(p1 + fleeVector * 100);

	return fleeVector;
}

bool ScoutManager::isValidFleePosition(BWAPI::Position pos) 
{

	// get x and y from the position
	int x(pos.x), y(pos.y);

	// walkable tiles exist every 8 pixels
	bool good = BWAPI::Broodwar->isWalkable(x/8, y/8);
	
	// if it's not walkable throw it out
	if (!good) return false;
	
	// for each of those units, if it's a building or an attacking enemy unit we don't want to go there
	for (BWAPI::Unit unit : BWAPI::Broodwar->getUnitsOnTile(x/32, y/32)) 
	{
		if	(unit->getType().isBuilding() || unit->getType().isResourceContainer() || 
			(unit->getPlayer() != BWAPI::Broodwar->self() && unit->getType().groundWeapon() != BWAPI::WeaponTypes::None)) 
		{		
				return false;
		}
	}

	int geyserDist = 50;
	int mineralDist = 32;

	BWTA::BaseLocation * enemyLocation = InformationManager::Instance().getMainBaseLocation(BWAPI::Broodwar->enemy());
    if (!enemyLocation)
    {
        return true;
    }
    
    UAB_ASSERT(enemyLocation, "Should know an enemy location");

    for (auto & unit : BWAPI::Broodwar->getAllUnits())
    {
        BWAPI::UnitType type = unit->getType();

        if (type == BWAPI::UnitTypes::Resource_Vespene_Geyser && unit->getDistance(pos) < geyserDist)
        {
            return false;
        }
        
        if (type == BWAPI::UnitTypes::Resource_Mineral_Field && unit->getDistance(pos) < mineralDist)
        {
            return false;
        }
    }

	// otherwise it's okay
	return true;
}

// fills the GroundThreat vector within a radius of a target
void ScoutManager::fillGroundThreats(std::vector<GroundThreat> & threats, BWAPI::Position target)
{
	// radius of caring
	const int radiusWeCareAbout(1000);
	const int radiusSq(radiusWeCareAbout * radiusWeCareAbout);

	// for each of the candidate units
	for (BWAPI::UnitInterface* e : BWAPI::Broodwar->enemy()->getUnits())
	{
		// if they're not within the radius of caring, who cares?
		const BWAPI::Position delta(e->getPosition() - target);
		if(delta.x * delta.x + delta.y * delta.y > radiusSq)
		{
			continue;
		}

		// default threat
		GroundThreat threat;
		threat.unit		= e;
		threat.weight	= 1;

		// get the air weapon of the unit
		BWAPI::WeaponType groundWeapon(e->getType().groundWeapon());

		// if it's a bunker, weight it as if it were 4 marines
		if(e->getType() == BWAPI::UnitTypes::Terran_Bunker)
		{
			groundWeapon	= BWAPI::WeaponTypes::Gauss_Rifle;
			threat.weight	= 4;
		}

		// weight the threat based on the highest DPS
		if(groundWeapon != BWAPI::WeaponTypes::None)
		{
			threat.weight *= (static_cast<double>(groundWeapon.damageAmount()) / groundWeapon.damageCooldown());
			threats.push_back(threat);
		}
	}
}

BWAPI::UnitInterface* ScoutManager::closestEnemyWorker()
{
	BWAPI::UnitInterface* enemyWorker = NULL;
	double maxDist = 0;

	
	BWAPI::UnitInterface* geyser = getEnemyGeyser();
	
	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->enemy()->getUnits())
	{
		if (unit->getType().isWorker() && unit->isConstructing())
		{
			return unit;
		}
	}

	// for each enemy worker
	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->enemy()->getUnits())
	{
		if (unit->getType().isWorker())
		{
			double dist = unit->getDistance(geyser);

			if (dist < 800 && dist > maxDist)
			{
				maxDist = dist;
				enemyWorker = unit;
			}
		}
	}

	return enemyWorker;
}

BWAPI::UnitInterface* ScoutManager::getEnemyGeyser()
{
	BWAPI::UnitInterface* geyser = NULL;
	BWTA::BaseLocation * enemyBaseLocation = InformationManager::Instance().getMainBaseLocation(BWAPI::Broodwar->enemy());

	for (BWAPI::UnitInterface* unit : enemyBaseLocation->getGeysers())
	{
		geyser = unit;
	}

	return geyser;
}

bool ScoutManager::enemyWorkerInRadius()
{
	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->enemy()->getUnits())
	{
		if (unit->getType().isWorker() && (unit->getDistance(workerScout) < 300))
		{
			return true;
		}
	}

	return false;
}

bool ScoutManager::immediateThreat()
{
	std::vector<BWAPI::UnitInterface *> enemyAttackingWorkers;
	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->enemy()->getUnits())
	{
		if (unit->getType().isWorker() && unit->isAttacking())
		{
			enemyAttackingWorkers.push_back(unit);
			if (Config::Debug::DrawUnitTargetInfo)
            {
                BWAPI::Broodwar->drawCircleMap(unit->getPosition().x, unit->getPosition().y, 5, BWAPI::Colors::Yellow);
            }
		}
	}
	
	if (workerScout->isUnderAttack())
	{
		return true;
	}

	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->enemy()->getUnits())
	{
		double dist = unit->getDistance(workerScout);
		double range = unit->getType().groundWeapon().maxRange();

		if (unit->getType().canAttack() && !unit->getType().isWorker() && (dist <= range + 32))
		{
			return true;
		}
	}

	return false;
}

void ScoutManager::smartMove(BWAPI::UnitInterface* attacker, BWAPI::Position targetPosition)
{
	// if we have issued a command to this unit already this frame, ignore this one
	if (attacker->getLastCommandFrame() >= BWAPI::Broodwar->getFrameCount())
	{
		return;
	}

	// get the unit's current command
	BWAPI::UnitCommand currentCommand(attacker->getLastCommand());

	// if we've already told this unit to attack this target, ignore this command
	if (currentCommand.getType() == BWAPI::UnitCommandTypes::Move && currentCommand.getTargetPosition() == targetPosition)
	{
		return;
	}

	// if nothing prevents it, attack the target
	attacker->move(targetPosition);
}

void ScoutManager::smartAttack(BWAPI::UnitInterface* attacker, BWAPI::UnitInterface* target)
{
	// if we have issued a command to this unit already this frame, ignore this one
	if (attacker->getLastCommandFrame() >= BWAPI::Broodwar->getFrameCount())
	{
		return;
	}

	// get the unit's current command
	BWAPI::UnitCommand currentCommand(attacker->getLastCommand());

	// if we've already told this unit to attack this target, ignore this command
	if (currentCommand.getType() == BWAPI::UnitCommandTypes::Attack_Unit && currentCommand.getTarget() == target)
	{
		return;
	}

	// if nothing prevents it, attack the target
	attacker->attack(target);
}