#include "Squad.h"

using namespace UAlbertaBot;

int  Squad::lastRetreatSwitch = 0;
bool Squad::lastRetreatSwitchVal = false;

Squad::Squad(const std::vector<BWAPI::UnitInterface *> & units, SquadOrder order) 
	: units(units)
	, order(order)
{
}

void Squad::update()
{
	// update all necessary unit information within this squad
	updateUnits();

	// determine whether or not we should regroup
	const bool needToRegroup(needsToRegroup());

	
	// draw some debug info
	if (Config::Debug::DrawSquadInfo && order.type == SquadOrder::Attack) 
	{
		BWAPI::Broodwar->drawTextScreen(200, 330, "%s", regroupStatus.c_str());

		BWAPI::UnitInterface* closest = unitClosestToEnemy();
		if (closest && (BWAPI::Broodwar->getFrameCount() % 24 == 0))
		{
			//BWAPI::Broodwar->setScreenPosition(closest->getPosition().x - 320, closest->getPosition().y - 200);
		}
	}

	// if we do need to regroup, do it
	if (needToRegroup)
	{
		InformationManager::Instance().lastFrameRegroup = 1;

		const BWAPI::Position regroupPosition(calcRegroupPosition());

        if (Config::Debug::DrawCombatSimulationInfo)
        {
		    BWAPI::Broodwar->drawTextScreen(200, 150, "REGROUP");
        }

		BWAPI::Broodwar->drawCircleMap(regroupPosition.x, regroupPosition.y, 30, BWAPI::Colors::Purple, true);

		meleeManager.regroup(regroupPosition);
		rangedManager.regroup(regroupPosition);
	}
	else // otherwise, execute micro
	{
		InformationManager::Instance().lastFrameRegroup = 1;

		meleeManager.execute(order);
		rangedManager.execute(order);
		transportManager.execute(order);

		detectorManager.setUnitClosestToEnemy(unitClosestToEnemy());
		detectorManager.execute(order);
	}
}

void Squad::updateUnits()
{
	setAllUnits();
	setNearEnemyUnits();
	addUnitsToMicroManagers();
}

void Squad::setAllUnits()
{
	// clean up the units vector just in case one of them died
	std::vector<BWAPI::UnitInterface *> goodUnits;
	for (BWAPI::UnitInterface* unit : units)
	{
		if( unit->isCompleted() && 
			unit->getHitPoints() > 0 && 
			unit->exists() &&
			unit->getPosition().isValid() &&
			unit->getType() != BWAPI::UnitTypes::Unknown)
		{
			goodUnits.push_back(unit);
		}
	}
	units = goodUnits;
}

void Squad::setNearEnemyUnits()
{
	nearEnemy.clear();
	for (BWAPI::UnitInterface* unit : units)
	{
		int x = unit->getPosition().x;
		int y = unit->getPosition().y;

		int left = unit->getType().dimensionLeft();
		int right = unit->getType().dimensionRight();
		int top = unit->getType().dimensionUp();
		int bottom = unit->getType().dimensionDown();

		nearEnemy[unit] = unitNearEnemy(unit);
		if (nearEnemy[unit])
		{
			if (Config::Debug::DrawSquadInfo) BWAPI::Broodwar->drawBoxMap(x-left, y - top, x + right, y + bottom, Config::Debug::ColorUnitNearEnemy);
		}
		else
		{
			if (Config::Debug::DrawSquadInfo) BWAPI::Broodwar->drawBoxMap(x-left, y - top, x + right, y + bottom, Config::Debug::ColorUnitNotNearEnemy);
		}
	}
}

void Squad::addUnitsToMicroManagers()
{
	std::vector<BWAPI::UnitInterface *> meleeUnits;
	std::vector<BWAPI::UnitInterface *> rangedUnits;
	std::vector<BWAPI::UnitInterface *> detectorUnits;
	std::vector<BWAPI::UnitInterface *> transportUnits;

	// add units to micro managers
	for (BWAPI::UnitInterface* unit : units)
	{
		if(unit->isCompleted() && unit->getHitPoints() > 0 && unit->exists())
		{
			// select dector units
			if (unit->getType().isDetector() && !unit->getType().isBuilding())
			{
				detectorUnits.push_back(unit);
			}
			// select transport units
			else if (unit->getType() == BWAPI::UnitTypes::Protoss_Shuttle || unit->getType() == BWAPI::UnitTypes::Terran_Dropship)
			{
				transportUnits.push_back(unit);
			}
			// select ranged units
			else if ((unit->getType().groundWeapon().maxRange() > 32) || (unit->getType() == BWAPI::UnitTypes::Protoss_Reaver))
			{
				rangedUnits.push_back(unit);
			}
			// select melee units
			else if (unit->getType().groundWeapon().maxRange() <= 32)
			{
				meleeUnits.push_back(unit);
			}
		}
	}

	meleeManager.setUnits(meleeUnits);
	rangedManager.setUnits(rangedUnits);
	detectorManager.setUnits(detectorUnits);
	transportManager.setUnits(detectorUnits);
}

// calculates whether or not to regroup
bool Squad::needsToRegroup()
{
	// if we are not attacking, never regroup
	if (units.empty() || (order.type != SquadOrder::Attack))
	{
		regroupStatus = std::string("\x04 No combat units available");
		return false;
	}


	BWAPI::UnitInterface* unitClosest = unitClosestToEnemy();

	if (!unitClosest)
	{
		regroupStatus = std::string("\x04 No closest unit");
		return false;
	}

    SparCraft::ScoreType score = 0;
	int SPARCRAFTscore = 0;

	//do the SparCraft Simulation!
	CombatSimulation sim;

	// special case with zealots vs. zerglings. combat simulation favours zerglings due to no unit collisions, check to see if we have 1/3 zealots
	if (sim.checkZealotVsZergling(unitClosest->getPosition(), Config::Micro::CombatRegroupRadius + InformationManager::Instance().lastFrameRegroup * 300))
	{
		regroupStatus = std::string("\x04 Attack - Zealot vs. Zergling - 1/3 condition met!");
		return false;
	}

	sim.setCombatUnits(unitClosest->getPosition(), Config::Micro::CombatRegroupRadius + InformationManager::Instance().lastFrameRegroup * 300);
	SPARCRAFTscore = sim.simulateCombat();

	score = SPARCRAFTscore;

	// if we are DT rushing and we haven't lost a DT yet, no retreat!
	if (Config::Strategy::StrategyName == "Protoss_DTRush" &&
		(BWAPI::Broodwar->self()->deadUnitCount(BWAPI::UnitTypes::Protoss_Dark_Templar) == 0))
	{
		regroupStatus = std::string("\x04 DARK TEMPLAR HOOOOO!");
		return false;
	}


    bool retreat = score < 0;
    int switchTime = 100;
    bool waiting = false;

    // we should not attack unless 5 seconds have passed since a retreat
    if (retreat != lastRetreatSwitchVal)
    {
        if (retreat == false && (BWAPI::Broodwar->getFrameCount() - lastRetreatSwitch < switchTime))
        {
            waiting = true;
            retreat = lastRetreatSwitchVal;
        }
        else
        {
            waiting = false;
            lastRetreatSwitch = BWAPI::Broodwar->getFrameCount();
            lastRetreatSwitchVal = retreat;
        }
    }
	
	if (retreat)
	{
		regroupStatus = std::string("\x04 Retreat - simulation predicts defeat");
	}
	else
	{
		regroupStatus = std::string("\x04 Attack - simulation predicts success");
	}

	return retreat;
}

void Squad::setSquadOrder(const SquadOrder & so)
{
	order = so;
}

bool Squad::unitNearEnemy(BWAPI::UnitInterface* unit)
{
	assert(unit);

	std::vector<BWAPI::UnitInterface *> enemyNear;

	MapGrid::Instance().GetUnits(enemyNear, unit->getPosition(), 400, false, true);

	return enemyNear.size() > 0;
}

BWAPI::Position Squad::calcCenter()
{
    if (units.empty())
    {
        if (Config::Debug::DrawSquadInfo)
        {
            BWAPI::Broodwar->printf("Squad::calcCenter() called on empty squad");
        }
        return BWAPI::Position(0,0);
    }

	BWAPI::Position accum(0,0);
	for (BWAPI::UnitInterface* unit : units)
	{
		accum += unit->getPosition();
	}
	return BWAPI::Position(accum.x / units.size(), accum.y / units.size());
}

BWAPI::Position Squad::calcRegroupPosition()
{
	BWAPI::Position regroup(0,0);

	int minDist(100000);

	for (BWAPI::UnitInterface* unit : units)
	{
		if (!nearEnemy[unit])
		{
			int dist = unit->getDistance(order.position);
			if (dist < minDist)
			{
				minDist = dist;
				regroup = unit->getPosition();
			}
		}
	}

	if (regroup == BWAPI::Position(0,0))
	{
		return BWTA::getRegion(BWTA::getStartLocation(BWAPI::Broodwar->self())->getTilePosition())->getCenter();
	}
	else
	{
		return regroup;
	}
}

BWAPI::UnitInterface* Squad::unitClosestToEnemy()
{
	BWAPI::UnitInterface* closest = NULL;
	int closestDist = 100000;

	for (BWAPI::UnitInterface* unit : units)
	{
		if (unit->getType() == BWAPI::UnitTypes::Protoss_Observer)
		{
			continue;
		}

		// the distance to the order position
		int dist = MapTools::Instance().getGroundDistance(unit->getPosition(), order.position);

		if (dist != -1 && (!closest || dist < closestDist))
		{
			closest = unit;
			closestDist = dist;
		}
	}

	if (!closest)
	{
		for (BWAPI::UnitInterface* unit : units)
		{
			if (unit->getType() == BWAPI::UnitTypes::Protoss_Observer)
			{
				continue;
			}

			// the distance to the order position
			int dist = unit->getDistance(BWAPI::Position(BWAPI::Broodwar->enemy()->getStartLocation()));

			if (dist != -1 && (!closest || dist < closestDist))
			{
				closest = unit;
				closestDist = dist;
			}
		}
	}

	return closest;
}

int Squad::squadUnitsNear(BWAPI::Position p)
{
	int numUnits = 0;

	for (BWAPI::UnitInterface* unit : units)
	{
		if (unit->getDistance(p) < 600)
		{
			numUnits++;
		}
	}

	return numUnits;
}

bool Squad::squadObserverNear(BWAPI::Position p)
{
	for (BWAPI::UnitInterface* unit : units)
	{
		if (unit->getType().isDetector() && unit->getDistance(p) < 300)
		{
			return true;
		}
	}

	return false;
}

const std::vector<BWAPI::UnitInterface *> &Squad::getUnits() const	
{ 
	return units; 
} 

const SquadOrder & Squad::getSquadOrder()	const			
{ 
	return order; 
}

void Squad::addUnit(BWAPI::UnitInterface *u)
{
	units.push_back(u);
}

bool Squad::removeUnit(BWAPI::UnitInterface *u)
{
	for (auto it = units.begin(); it != units.end(); it++){
		if ((*it)->getID() == u->getID()){
			units.erase(it);
			return true;
		}
	}
	return false;
}