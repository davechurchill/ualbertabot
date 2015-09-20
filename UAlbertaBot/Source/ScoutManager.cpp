#include "ScoutManager.h"
#include "ProductionManager.h"

using namespace UAlbertaBot;

ScoutManager::ScoutManager() 
    : workerScout(NULL)
    , numWorkerScouts(0)
    , scoutUnderAttack(false)
    , gasStealStatus("None")
    , scoutStatus("None")
    , didGasSteal(false)
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
    drawScoutInformation(200, 320);
}

void ScoutManager::drawScoutInformation(int x, int y)
{
    if (!Config::Debug::DrawScoutInfo)
    {
        return;
    }

    BWAPI::Broodwar->drawTextScreen(x, y, "ScoutInfo: %s", scoutStatus.c_str());
    BWAPI::Broodwar->drawTextScreen(x, y+10, "GasSteal: %s", gasStealStatus.c_str());
}

void ScoutManager::moveScouts()
{
	if (!workerScout || !workerScout->exists() || !(workerScout->getHitPoints() > 0))
	{
		return;
	}
    
    gasSteal();

	// get the enemy base location, if we have one
	BWTA::BaseLocation * enemyBaseLocation = InformationManager::Instance().getMainBaseLocation(BWAPI::Broodwar->enemy());

    int scoutDistanceThreshold = 30;

    if (didGasSteal)
    {
        return;
    }

	// if we know where the enemy region is and where our scout is
	if (workerScout && enemyBaseLocation)
	{
        int scoutDistanceToEnemy = MapTools::Instance().getGroundDistance(workerScout->getPosition(), enemyBaseLocation->getPosition());
        bool scoutInRangeOfenemy = scoutDistanceToEnemy <= scoutDistanceThreshold;
        
        // we only care if the scout is under attack within the enemy region
        // this ignores if their scout worker attacks it on the way to their base
        if (workerScout->isUnderAttack() && scoutInRangeOfenemy)
        {
	        scoutUnderAttack = true;
        }

        if (!workerScout->isUnderAttack() && !enemyWorkerInRadius())
        {
	        scoutUnderAttack = false;
        }

		// if the scout is in the enemy region
		if (scoutInRangeOfenemy)
		{
			// get the closest enemy worker
			BWAPI::UnitInterface* closestWorker = closestEnemyWorker();

			// if the worker scout is not under attack
			if (!scoutUnderAttack)
			{
                scoutStatus = "Harass enemy worker";
				// if there is a worker nearby, harass it
				if (!Config::Strategy::GasStealWithScout && closestWorker && (workerScout->getDistance(closestWorker) < 800))
				{
					MicroManager::SmartAttackUnit(workerScout, closestWorker);
				}
				// otherwise keep moving to the enemy region
				else
				{
                    scoutStatus = "Exploring enemy Region";
					// move to the enemy region
					MicroManager::SmartMove(workerScout, enemyBaseLocation->getPosition());
					BWAPI::Broodwar->drawLineMap(workerScout->getPosition().x, workerScout->getPosition().y, 
						enemyBaseLocation->getPosition().x, enemyBaseLocation->getPosition().y,
						BWAPI::Colors::Yellow);
				}
				
			}
			// if the worker scout is under attack
			else
			{
                scoutStatus = "Under attack inside, fleeing";
                BWAPI::Position fleeTo = BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation());

				MicroManager::SmartMove(workerScout, fleeTo);
			}
		}
		// if the scout is not in the enemy region
		else if (scoutUnderAttack)
		{
            scoutStatus = "Under attack outisde, going home";

			MicroManager::SmartMove(workerScout, BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()));
		}
		else
		{
            scoutStatus = "Enemy region known, going there";

			// move to the enemy region
			MicroManager::SmartMove(workerScout, enemyBaseLocation->getPosition());	
		}
		
	}

	// for each start location in the level
	if (!enemyBaseLocation)
	{
        scoutStatus = "Enemy base unknown, exploring";

		for (BWTA::BaseLocation * startLocation : BWTA::getStartLocations()) 
		{
			// if we haven't explored it yet
			if (!BWAPI::Broodwar->isExplored(startLocation->getTilePosition())) 
			{
				// assign a zergling to go scout it
				MicroManager::SmartMove(workerScout, BWAPI::Position(startLocation->getTilePosition()));			
				return;
			}
		}
	}
}

void ScoutManager::gasSteal()
{
    if (!Config::Strategy::GasStealWithScout)
    {
        gasStealStatus = "Not using gas steal";
        return;
    }

    if (didGasSteal)
    {
        return;
    }

    if (!workerScout)
    {
        gasStealStatus = "No worker scout";
        return;
    }

    BWTA::BaseLocation * enemyBaseLocation = InformationManager::Instance().getMainBaseLocation(BWAPI::Broodwar->enemy());
    if (!enemyBaseLocation)
    {
        gasStealStatus = "No enemy base location found";
        return;
    }

    int scoutDistanceToEnemy = MapTools::Instance().getGroundDistance(workerScout->getPosition(), enemyBaseLocation->getPosition());
    
    BWAPI::UnitInterface * enemyGeyser = getEnemyGeyser();
    if (!enemyGeyser)
    {
        gasStealStatus = "No enemy geyser found";
        false;
    }

    if (!didGasSteal)
    {
        ProductionManager::Instance().queueGasSteal();
        didGasSteal = true;
        MicroManager::SmartMove(workerScout, enemyGeyser->getPosition());
        gasStealStatus = "Did Gas Steal";
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