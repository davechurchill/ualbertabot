#include "Squad.h"

using namespace UAlbertaBot;

int  Squad::lastRetreatSwitch = 0;
int	 Squad::lastRegroupSwitch = 0;
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
	// every second 
	// every 2 frames
	int switchTime = 0;
	bool needToRegroup = false;

	if ((BWAPI::Broodwar->getFrameCount() - lastRegroupSwitch < switchTime))
	{
		; //do nothing	
	}
	else
	{
		needToRegroup = needsToRegroup();
		lastRegroupSwitch = BWAPI::Broodwar->getFrameCount();
	}
	
	
	// draw some debug info
	if (Options::Debug::DRAW_UALBERTABOT_DEBUG && order.type == SquadOrder::Attack) 
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
		BWAPI::Broodwar->drawTextScreen(200, 150, "REGROUP");

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
			if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawBoxMap(x-left, y - top, x + right, y + bottom, Options::Debug::COLOR_UNIT_NEAR_ENEMY);
		}
		else
		{
			if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawBoxMap(x-left, y - top, x + right, y + bottom, Options::Debug::COLOR_UNIT_NOTNEAR_ENEMY);
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

	//which one to use?
	if (Options::Modules::USING_COMBAT_PREDICTOR && CombatPredictor::Instance().vsTrainedOpp)
	{
		//I have trained against this opponent! 
		//can use combat predictor
	}
	else
	{
		//do the SparCraft Simulation!
		CombatSimulation sim;

		// special case with zealots vs. zerglings. combat simulation favours zerglings due to no unit collisions, check to see if we have 1/3 zealots
		if (sim.checkZealotVsZergling(unitClosest->getPosition(), Options::Micro::COMBAT_REGROUP_RADIUS + InformationManager::Instance().lastFrameRegroup * 300))
		{
			regroupStatus = std::string("\x04 Attack - Zealot vs. Zergling - 1/3 condition met!");
			return false;
		}

		sim.setCombatUnits(unitClosest->getPosition(), Options::Micro::COMBAT_REGROUP_RADIUS + InformationManager::Instance().lastFrameRegroup * 300);
		SPARCRAFTscore = sim.simulateCombat();

		score = SPARCRAFTscore;
	}


    if (Options::Modules::USING_COMBAT_PREDICTOR)
    {
        bool dmgHasBeenDone = false;

        //find units involved
        const BWAPI::Position center = unitClosest->getPosition();
        const int radius = Options::Micro::COMBAT_REGROUP_RADIUS + InformationManager::Instance().lastFrameRegroup * 300;
        BWAPI::Broodwar->drawCircleMap(center.x, center.y, 10, BWAPI::Colors::Red, true);

        std::vector<BWAPI::UnitInterface*> ourCombatUnits;
        std::vector<UnitInfo> enemyCombatUnits;

        MapGrid::Instance().GetUnits(ourCombatUnits, center, Options::Micro::COMBAT_REGROUP_RADIUS, true, false);
        InformationManager::Instance().getNearbyForce(enemyCombatUnits, center, BWAPI::Broodwar->enemy(), Options::Micro::COMBAT_REGROUP_RADIUS);

        //add units 
        std::vector<std::pair<int, int>> ArmyA;
        std::vector<std::pair<int, int>> ArmyB;

		//enemy DTs and observers? 
		//TODO: can be coded faster
		bool oppHasOBS = false;
		bool oppHasDTs = false;
		for (UnitInfo & ui : enemyCombatUnits)
		{
			if (ui.completed)
			{
				if (ui.type.isDetector())
					oppHasOBS = true;
				else if (ui.type == BWAPI::UnitTypes::Protoss_Dark_Templar)
					oppHasDTs = true;
			}
		}


		bool IhaveOBS = false;
		bool IhaveDTs = false;

        for (BWAPI::UnitInterface* unit : ourCombatUnits)
        {
            if (unit->isAttacking() || unit->isUnderAttack())
                dmgHasBeenDone = true;


            BWAPI::UnitType type = unit->getType();
			if (type.isDetector()) IhaveOBS = true;
			else if (type == BWAPI::UnitTypes::Protoss_Dark_Templar) IhaveDTs = true;

            if (type.canAttack() || type.isWorker()
				|| type.isDetector()
                || type == BWAPI::UnitTypes::Terran_Medic
                || type == BWAPI::UnitTypes::Terran_Bunker)
            {
				//visible DTs have the unit id 61 
				//invisible DTs will have the id 74 
				if (type == BWAPI::UnitTypes::Protoss_Dark_Templar && !oppHasOBS)
					ArmyA.push_back(std::make_pair(74, unit->getHitPoints() + unit->getShields()));
				else
					ArmyA.push_back(std::make_pair(type.getID(), unit->getHitPoints()+unit->getShields()));
            }
        }


		for (UnitInfo & ui : enemyCombatUnits)
		{
			BWAPI::UnitType type = ui.type;
			//type.ifFlyer()
			if (ui.completed &&
				(type.canAttack() || type.isWorker()
				|| type.isDetector()
				|| type == BWAPI::UnitTypes::Terran_Medic
				|| type == BWAPI::UnitTypes::Terran_Bunker))
			{
				//visible DTs have the unit id 61 
				//invisible DTs will have the id 74 
				if (type == BWAPI::UnitTypes::Protoss_Dark_Templar)
				{
					
					if (!IhaveOBS) //I can't see it, it's invisible
					{
						int hp_last = ui.lastHealth; 
						if (hp_last < 1) //if I saw it before keep the hp otherwise max hp for DT
							hp_last = 120;
						ArmyB.push_back(std::make_pair(type.getID() + 13, hp_last));
					}
					else //I can see it, normal DT stats
						ArmyB.push_back(std::make_pair(type.getID(), ui.lastHealth));
				}
				else	
					ArmyB.push_back(std::make_pair(type.getID(), ui.lastHealth));

				//if (ui.canCloak())
				//{
				//	int hp = CombatPredictor::Instance().observedHPs[ui.unitID];
				//	;
				//}
			}
		}

		//predict outcome
		score = CombatPredictor::Instance().predictCombat(ArmyA, ArmyB);

		if (Options::Modules::USING_BATTLE_LOG && dmgHasBeenDone) //actual fight! 
		{
			//do the SparCraft Simulation! (To compare against)
			CombatSimulation sim;

			// special case with zealots vs. zerglings. combat simulation favours zerglings due to no unit collisions, check to see if we have 1/3 zealots
			if (sim.checkZealotVsZergling(unitClosest->getPosition(), Options::Micro::COMBAT_REGROUP_RADIUS + InformationManager::Instance().lastFrameRegroup * 300))
			{
				regroupStatus = std::string("\x04 Attack - Zealot vs. Zergling - 1/3 condition met!");
				return false;
			}

			sim.setCombatUnits(unitClosest->getPosition(), Options::Micro::COMBAT_REGROUP_RADIUS + InformationManager::Instance().lastFrameRegroup * 300);
			SPARCRAFTscore = sim.simulateCombat();

			//save combat, will have to update every frame
			Combat newCombat(ourCombatUnits, enemyCombatUnits, IhaveOBS, oppHasOBS);
			if (!newCombat.isFinished()) //IF VALID
			{
				newCombat.MATLAB_Prediction = score;
				newCombat.SparcraftPrediction = SPARCRAFTscore;
				CombatPredictor::Instance().addCombat(newCombat);
			}
		}
    }


	// if we are DT rushing and we haven't lost a DT yet, no retreat!
	if (StrategyManager::Instance().getCurrentStrategy() == StrategyManager::ProtossDarkTemplar &&
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
        if (Options::Debug::DRAW_UALBERTABOT_DEBUG)
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