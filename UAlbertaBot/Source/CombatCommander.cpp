#include "CombatCommander.h"

using namespace UAlbertaBot;

CombatCommander::CombatCommander() 
	: attacking(false)
	, foundEnemy(false)
	, attackSent(false) 
    , selfTotalDeadUnits(0)
    , numUnitsNeededForAttack(0)
{
	
}

bool CombatCommander::squadUpdateFrame()
{
	return BWAPI::Broodwar->getFrameCount() % 24 == 0;
}

void CombatCommander::update(std::set<BWAPI::UnitInterface*> unitsToAssign)
{
	if(squadUpdateFrame())
	{
		// clear all squad data
		squadData.clearSquadData();

		// give back combat workers to worker manager
		WorkerManager::Instance().finishedWithCombatWorkers();
        
		// Assign defense and attack squads
        assignScoutDefenseSquads();
		assignDefenseSquads(unitsToAssign);
		assignAttackSquads(unitsToAssign);
		assignIdleSquads(unitsToAssign);
	}

	squadData.update();
}

void CombatCommander::assignIdleSquads(std::set<BWAPI::UnitInterface*> & unitsToAssign)
{
	if (unitsToAssign.empty()) { return; }

	std::vector<BWAPI::UnitInterface *> combatUnits(unitsToAssign.begin(), unitsToAssign.end());
	unitsToAssign.clear();

	squadData.addSquad(Squad(combatUnits, SquadOrder(SquadOrder::Defend, BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()), 1000, "Defend Idle")));
}

void CombatCommander::assignAttackSquads(std::set<BWAPI::UnitInterface*> & unitsToAssign)
{
	if (unitsToAssign.empty()) { return; }

	bool workersDefending = false;
	for (BWAPI::UnitInterface* unit : unitsToAssign)
	{
		if (unit->getType().isWorker())
		{
			workersDefending = true;
		}
	}

	// do we have workers in combat
	bool attackEnemy = !unitsToAssign.empty() && !workersDefending && StrategyManager::Instance().doAttack(unitsToAssign);

	// if we are attacking, what area are we attacking?
	if (attackEnemy) 
	{	
		assignAttackRegion(unitsToAssign);				// attack occupied enemy region
		assignAttackKnownBuildings(unitsToAssign);		// attack known enemy buildings
		assignAttackVisibleUnits(unitsToAssign);			// attack visible enemy units
		assignAttackExplore(unitsToAssign);				// attack and explore for unknown units
	} 
}

BWTA::Region * CombatCommander::getClosestEnemyRegion()
{
	BWTA::Region * closestEnemyRegion = NULL;
	double closestDistance = 100000;

	// for each region that our opponent occupies
	for (BWTA::Region * region : InformationManager::Instance().getOccupiedRegions(BWAPI::Broodwar->enemy()))
	{
		double distance = region->getCenter().getDistance(BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()));

		if (!closestEnemyRegion || distance < closestDistance)
		{
			closestDistance = distance;
			closestEnemyRegion = region;
		}
	}

	return closestEnemyRegion;
}

void CombatCommander::assignScoutDefenseSquads() 
{
	// for each of our occupied regions
	for (BWTA::Region * myRegion : InformationManager::Instance().getOccupiedRegions(BWAPI::Broodwar->self()))
	{
		BWAPI::Position regionCenter = myRegion->getCenter();
		if (!regionCenter.isValid())
		{
			continue;
		}

		// all of the enemy units in this region
		std::set<BWAPI::UnitInterface*> enemyUnitsInRegion;
		for (BWAPI::UnitInterface* enemyUnit : BWAPI::Broodwar->enemy()->getUnits())
		{			
			if (BWTA::getRegion(BWAPI::TilePosition(enemyUnit->getPosition())) == myRegion)
			{
				enemyUnitsInRegion.insert(enemyUnit);
			}
		}

        // special case: figure out if the only attacker is a worker, the enemy is scouting
        if (enemyUnitsInRegion.size() == 1 && (*enemyUnitsInRegion.begin())->getType().isWorker())
        {
            // the enemy worker that is attacking us
            BWAPI::UnitInterface* enemyWorker       = *enemyUnitsInRegion.begin();

            // get our worker unit that is mining that is closest to it
            BWAPI::UnitInterface* workerDefender    = WorkerManager::Instance().getClosestMineralWorkerTo(enemyWorker);

			if (workerDefender)//only if we found a mineral worker
			{
				// grab it from the worker manager
				WorkerManager::Instance().setCombatWorker(workerDefender);

				// put it into a unit vector
				std::vector<BWAPI::UnitInterface *> workerDefenseForce;
				workerDefenseForce.push_back(workerDefender);

				// make a squad using the worker to defend
				squadData.addSquad(Squad(workerDefenseForce, SquadOrder(SquadOrder::Defend, regionCenter, 1000, "Get That Scout!")));
			}
			return;
        }
	}
}

void CombatCommander::assignDefenseSquads(std::set<BWAPI::UnitInterface*> & unitsToAssign) 
{
	if (unitsToAssign.empty()) { return; }

	// for each of our occupied regions
	for (BWTA::Region * myRegion : InformationManager::Instance().getOccupiedRegions(BWAPI::Broodwar->self()))
	{
		BWAPI::Position regionCenter = myRegion->getCenter();
		if (!regionCenter.isValid())
		{
			continue;
		}

		// start off assuming all enemy units in region are just workers
		int numDefendersPerEnemyUnit = 1;

        int numEnemyFlyingInRegion = 0;
        int numEnemyGroundInRegion = 0;

		// all of the enemy units in this region
		std::set<BWAPI::UnitInterface*> enemyUnitsInRegion;
		for (BWAPI::UnitInterface* enemyUnit : BWAPI::Broodwar->enemy()->getUnits())
		{			
			if (BWTA::getRegion(BWAPI::TilePosition(enemyUnit->getPosition())) == myRegion)
			{
                if (enemyUnit->isLifted() || enemyUnit->getType().isFlyer())
                {
                    numEnemyFlyingInRegion++;
                }
                else if (!enemyUnit->getType().isWorker())
                {
                    numEnemyGroundInRegion++;
                }

				enemyUnitsInRegion.insert(enemyUnit);

				// if the enemy isn't a worker, increase the amount of defenders for it
				if (!enemyUnit->getType().isWorker())
				{
					numDefendersPerEnemyUnit = 1;
				}
			}
		}

		// figure out how many units we need on defense
		const int numFlyingNeeded = numDefendersPerEnemyUnit * numEnemyFlyingInRegion;
		const int numGroundNeeded = numDefendersPerEnemyUnit * numEnemyGroundInRegion;

		if(numGroundNeeded > 0 || numFlyingNeeded > 0)
		{
			// our defenders
			std::set<BWAPI::UnitInterface*> flyingDefenders;
			std::set<BWAPI::UnitInterface*> groundDefenders;

			for (BWAPI::UnitInterface* unit : unitsToAssign)
			{
				if (unit->getType().airWeapon() != BWAPI::WeaponTypes::None)
				{
					flyingDefenders.insert(unit);
				}
				else if (unit->getType().groundWeapon() != BWAPI::WeaponTypes::None)
				{
					groundDefenders.insert(unit);
				}
			}

			// the defense force we want to send
			std::vector<BWAPI::UnitInterface *> defenseForce;

			// get flying defenders
			for (int i=0; i<numFlyingNeeded && !flyingDefenders.empty(); ++i)
			{
				BWAPI::UnitInterface* flyingDefender = findClosestDefender(enemyUnitsInRegion, flyingDefenders);
				defenseForce.push_back(flyingDefender);
				unitsToAssign.erase(flyingDefender);
				flyingDefenders.erase(flyingDefender);
			}

			// get ground defenders
			for (int i=0; i<numGroundNeeded && !groundDefenders.empty(); ++i)
			{
				BWAPI::UnitInterface* groundDefender = findClosestDefender(enemyUnitsInRegion, groundDefenders);

				if (groundDefender->getType().isWorker())
				{
					WorkerManager::Instance().setCombatWorker(groundDefender);
				}

				defenseForce.push_back(groundDefender);
				unitsToAssign.erase(groundDefender);
				groundDefenders.erase(groundDefender);
			}

			// if we need a defense force, make the squad and give the order
			if (!defenseForce.empty()) 
			{
				squadData.addSquad(Squad(defenseForce, SquadOrder(SquadOrder::Defend, regionCenter, 1000, "Defend Region")));
				return;
			}
		}
	}
}

void CombatCommander::assignAttackRegion(std::set<BWAPI::UnitInterface*> & unitsToAssign) 
{
	if (unitsToAssign.empty()) { return; }

	BWTA::Region * enemyRegion = getClosestEnemyRegion();

	if (enemyRegion && enemyRegion->getCenter().isValid()) 
	{
		std::vector<BWAPI::UnitInterface *> oppUnitsInArea, ourUnitsInArea;
		MapGrid::Instance().GetUnits(oppUnitsInArea, enemyRegion->getCenter(), 800, false, true);
		MapGrid::Instance().GetUnits(ourUnitsInArea, enemyRegion->getCenter(), 200, true, false);

		if (!oppUnitsInArea.empty())
		{
			std::vector<BWAPI::UnitInterface *> combatUnits(unitsToAssign.begin(), unitsToAssign.end());
			unitsToAssign.clear();

			squadData.addSquad(Squad(combatUnits, SquadOrder(SquadOrder::Attack, enemyRegion->getCenter(), 1000, "Attack Region")));
		}
	}
}

void CombatCommander::assignAttackVisibleUnits(std::set<BWAPI::UnitInterface*> & unitsToAssign) 
{
	if (unitsToAssign.empty()) { return; }

	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->enemy()->getUnits())
	{
		if (unit->isVisible())
		{
			std::vector<BWAPI::UnitInterface *> combatUnits(unitsToAssign.begin(), unitsToAssign.end());
			unitsToAssign.clear();

			squadData.addSquad(Squad(combatUnits, SquadOrder(SquadOrder::Attack, unit->getPosition(), 1000, "Attack Visible")));

			return;
		}
	}
}

void CombatCommander::assignAttackKnownBuildings(std::set<BWAPI::UnitInterface*> & unitsToAssign) 
{
	if (unitsToAssign.empty()) { return; }

	FOR_EACH_UIMAP_CONST (iter, InformationManager::Instance().getUnitInfo(BWAPI::Broodwar->enemy()))
	{
		const UnitInfo ui(iter->second);
		if(ui.type.isBuilding())
		{
			std::vector<BWAPI::UnitInterface *> combatUnits(unitsToAssign.begin(), unitsToAssign.end());
			unitsToAssign.clear();

			squadData.addSquad(Squad(combatUnits, SquadOrder(SquadOrder::Attack, ui.lastPosition, 1000, "Attack Known")));
			return;	
		}
	}
}

void CombatCommander::assignAttackExplore(std::set<BWAPI::UnitInterface*> & unitsToAssign) 
{
	if (unitsToAssign.empty()) { return; }

	std::vector<BWAPI::UnitInterface *> combatUnits(unitsToAssign.begin(), unitsToAssign.end());
	unitsToAssign.clear();

	squadData.addSquad(Squad(combatUnits, SquadOrder(SquadOrder::Attack, MapGrid::Instance().getLeastExplored(), 1000, "Attack Explore")));
}

BWAPI::UnitInterface* CombatCommander::findClosestDefender(std::set<BWAPI::UnitInterface*> & enemyUnitsInRegion, const std::set<BWAPI::UnitInterface*> & units) 
{
	BWAPI::UnitInterface* closestUnit = NULL;
	double minDistance = 1000000;

	for (BWAPI::UnitInterface* enemyUnit : enemyUnitsInRegion) 
	{
		for (BWAPI::UnitInterface* unit : units)
		{
			if (!unit)
			{
				continue;
			}
			double dist = unit->getDistance(enemyUnit);
			if (!closestUnit || dist < minDistance) 
			{
				closestUnit = unit;
				minDistance = dist;
			}
		}
	}

	return closestUnit;
}

BWAPI::Position CombatCommander::getDefendLocation()
{
	return BWTA::getRegion(BWTA::getStartLocation(BWAPI::Broodwar->self())->getTilePosition())->getCenter();
}

void CombatCommander::drawSquadInformation(int x, int y)
{
	squadData.drawSquadInformation(x, y);
}