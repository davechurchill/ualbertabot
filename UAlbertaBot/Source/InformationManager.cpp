#include "Common.h"
#include "InformationManager.h"
#include "CombatPredictor.h"

using namespace UAlbertaBot;

#define SELF_INDEX 0
#define ENEMY_INDEX 1

// constructor
InformationManager::InformationManager() 
	: goForIt(false)
	, map(BWAPI::Broodwar)
	, lastFrameRegroup(false)
{
	initializeRegionInformation();
}

// get an instance of this
InformationManager & InformationManager::Instance() 
{
	static InformationManager instance;
	return instance;
}

void InformationManager::update() 
{
	updateUnitInfo();
	updateBaseLocationInfo();
	map.setUnitData(BWAPI::Broodwar);
	map.setBuildingData(BWAPI::Broodwar);
}

void InformationManager::updateUnitInfo() 
{
	// update enemy unit information
	for (BWAPI::UnitInterface * unit : BWAPI::Broodwar->enemy()->getUnits())
	{
		updateUnit(unit);
		if(Options::Modules::USING_COMBAT_PREDICTOR)
			CombatPredictor::Instance().observedHPs[unit->getID()] = unit->getHitPoints() + unit->getShields();
	}

	// update enemy unit information
	for (BWAPI::UnitInterface * unit : BWAPI::Broodwar->self()->getUnits())
	{
		updateUnit(unit);
	}

	// remove bad enemy units
	enemyUnitData.removeBadUnits();
	selfUnitData.removeBadUnits();
}

void InformationManager::initializeRegionInformation() 
{
	// set initial pointers to null
	mainBaseLocations[SELF_INDEX] = BWTA::getStartLocation(BWAPI::Broodwar->self());
	mainBaseLocations[ENEMY_INDEX] = BWTA::getStartLocation(BWAPI::Broodwar->enemy());

    mainBaseTilePositions[SELF_INDEX] = BWAPI::Broodwar->self()->getStartLocation();
    mainBaseTilePositions[ENEMY_INDEX] = BWAPI::Broodwar->enemy()->getStartLocation();

	// push that region into our occupied vector
	updateOccupiedRegions(BWTA::getRegion(mainBaseLocations[SELF_INDEX]->getTilePosition()), BWAPI::Broodwar->self());
}


void InformationManager::updateBaseLocationInfo() 
{
	occupiedRegions[SELF_INDEX].clear();
	occupiedRegions[ENEMY_INDEX].clear();
		
	// if we haven't found the enemy main base location yet
	if (!mainBaseLocations[ENEMY_INDEX]) 
	{ 
		// how many start locations have we explored
		int exploredStartLocations = 0;
		bool baseFound = false;

		// an undexplored base location holder
		BWTA::BaseLocation * unexplored = NULL;

		for (BWTA::BaseLocation * startLocation : BWTA::getStartLocations()) 
		{
			if (isEnemyBuildingInRegion(BWTA::getRegion(startLocation->getTilePosition()))) 
			{
				baseFound = true;
				BWAPI::Broodwar->printf("Enemy base found by seeing it");
				mainBaseLocations[ENEMY_INDEX] = startLocation;
				updateOccupiedRegions(BWTA::getRegion(startLocation->getTilePosition()), BWAPI::Broodwar->enemy());
			}

			// if it's explored, increment
			if (BWAPI::Broodwar->isExplored(startLocation->getTilePosition())) 
			{
				exploredStartLocations++;

			// otherwise set the unexplored base
			} 
			else 
			{
				unexplored = startLocation;
			}
		}

		// if we've explored every start location except one, it's the enemy
		if (!baseFound && exploredStartLocations == ((int)BWTA::getStartLocations().size() - 1)) 
		{
			BWAPI::Broodwar->printf("Enemy base found by process of elimination");
			mainBaseLocations[ENEMY_INDEX] = unexplored;
			updateOccupiedRegions(BWTA::getRegion(unexplored->getTilePosition()), BWAPI::Broodwar->enemy());
		}

	// otherwise we do know it, so push it back
	} 
	else 
	{
		updateOccupiedRegions(BWTA::getRegion(mainBaseLocations[ENEMY_INDEX]->getTilePosition()), BWAPI::Broodwar->enemy());
	}

	// for each enemy unit we know about
	FOR_EACH_UIMAP_CONST(iter, enemyUnitData.getUnits())
	{
		const UnitInfo & ui(iter->second);
		BWAPI::UnitType type = ui.type;

		// if the unit is a building
		if (type.isBuilding()) 
		{
			// update the enemy occupied regions
			updateOccupiedRegions(BWTA::getRegion(BWAPI::TilePosition(ui.lastPosition)), BWAPI::Broodwar->enemy());
		}
	}

	// for each of our units
	FOR_EACH_UIMAP_CONST(iter, selfUnitData.getUnits())
	{
		const UnitInfo & ui(iter->second);
		BWAPI::UnitType type = ui.type;

		// if the unit is a building
		if (type.isBuilding()) 
		{
			// update the enemy occupied regions
			updateOccupiedRegions(BWTA::getRegion(BWAPI::TilePosition(ui.lastPosition)), BWAPI::Broodwar->self());
		}
	}
}

void InformationManager::updateOccupiedRegions(BWTA::Region * region, BWAPI::PlayerInterface * player) 
{
	// if the region is valid (flying buildings may be in NULL regions)
	if (region)
	{
		// add it to the list of occupied regions
		occupiedRegions[getIndex(player)].insert(region);
	}
}

int InformationManager::getIndex(BWAPI::PlayerInterface * player)
{
	return player == BWAPI::Broodwar->self() ? SELF_INDEX : ENEMY_INDEX;
}

bool InformationManager::isEnemyBuildingInRegion(BWTA::Region * region) 
{
	// invalid regions aren't considered the same, but they will both be null
	if (!region)
	{
		return false;
	}

	FOR_EACH_UIMAP_CONST(iter, enemyUnitData.getUnits())
	{
		const UnitInfo & ui(iter->second);
		if (ui.type.isBuilding()) 
		{
			if (BWTA::getRegion(BWAPI::TilePosition(ui.lastPosition)) == region) 
			{
				return true;
			}
		}
	}

	return false;
}

int InformationManager::numEnemyUnitsInRegion(BWTA::Region * region) 
{
	// invalid region matching 
	if (!region)
	{
		return 0;
	}

	int unitsInRegion(0);
	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->enemy()->getUnits()) 
	{
		if (unit->isVisible() && BWTA::getRegion(BWAPI::TilePosition(unit->getPosition())) == region) 
		{
			unitsInRegion++;
		}
	}

	return unitsInRegion;
}

const UIMap & InformationManager::getUnitInfo(BWAPI::PlayerInterface * player) const
{
	return getUnitData(player).getUnits();
}

int InformationManager::numEnemyFlyingUnitsInRegion(BWTA::Region * region) 
{
	int unitsInRegion(0);
	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->enemy()->getUnits()) 
	{
		if (unit->isVisible() && BWTA::getRegion(BWAPI::TilePosition(unit->getPosition())) == region && unit->getType().isFlyer()) 
		{
			unitsInRegion++;
		}
	}

	return unitsInRegion;
}

std::set<BWTA::Region *> & InformationManager::getOccupiedRegions(BWAPI::PlayerInterface * player)
{
	return occupiedRegions[getIndex(player)];
}

BWTA::BaseLocation * InformationManager::getMainBaseLocation(BWAPI::PlayerInterface * player) 
{
	return mainBaseLocations[getIndex(player)];
}

void InformationManager::drawUnitInformation(int x, int y) {
	
	std::string prefix = "\x04";

	if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextScreen(x, y-10, "\x03Lost:\x04 S \x1f%d \x07%d\x04 E \x1f%d \x07%d ", 
		selfUnitData.getMineralsLost(), selfUnitData.getGasLost(), enemyUnitData.getMineralsLost(), enemyUnitData.getGasLost());
	if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextScreen(x, y, "\x04 Enemy Unit Information: %s", BWAPI::Broodwar->enemy()->getRace().getName().c_str());
	if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextScreen(x, y+20, "\x04UNIT NAME");
	if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextScreen(x+140, y+20, "\x04#");
	if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextScreen(x+160, y+20, "\x04X");

	int yspace = 0;

	// for each unit in the queue
	for (BWAPI::UnitType t : BWAPI::UnitTypes::allUnitTypes()) 
	{
		int numUnits = enemyUnitData.getNumUnits(t);
		int numDeadUnits = enemyUnitData.getNumDeadUnits(t);

		// if there exist units in the vector
		if (numUnits > 0) 
		{
			if (t.isDetector())			{ prefix = "\x10"; }		
			else if (t.canAttack())		{ prefix = "\x08"; }		
			else if (t.isBuilding())	{ prefix = "\x03"; }
			else						{ prefix = "\x04"; }

			if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextScreen(x, y+40+((yspace)*10), "%s%s", prefix.c_str(), t.getName().c_str());
			if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextScreen(x+140, y+40+((yspace)*10), "%s%d", prefix.c_str(), numUnits);
			if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextScreen(x+160, y+40+((yspace++)*10), "%s%d", prefix.c_str(), numDeadUnits);
		}
	}
}

void InformationManager::drawMapInformation(){//for now it's just BWTA information.
	//we will iterate through all the base locations, and draw their outlines.
	for (std::set<BWTA::BaseLocation*>::const_iterator i = BWTA::getBaseLocations().begin(); i != BWTA::getBaseLocations().end(); i++)
	{
		BWAPI::TilePosition p = (*i)->getTilePosition();
		BWAPI::Position c = (*i)->getPosition();

		//draw outline of center location
		BWAPI::Broodwar->drawBoxMap(p.x * 32, p.y * 32, p.x * 32 + 4 * 32, p.y * 32 + 3 * 32, BWAPI::Colors::Blue);

		//draw a circle at each mineral patch
		for (BWAPI::Unitset::iterator j = (*i)->getStaticMinerals().begin(); j != (*i)->getStaticMinerals().end(); j++)
		{
			BWAPI::Position q = (*j)->getInitialPosition();
			BWAPI::Broodwar->drawCircleMap(q.x, q.y, 30, BWAPI::Colors::Cyan);
		}

		//draw the outlines of vespene geysers
		for (BWAPI::Unitset::iterator j = (*i)->getGeysers().begin(); j != (*i)->getGeysers().end(); j++)
		{
			BWAPI::TilePosition q = (*j)->getInitialTilePosition();
			BWAPI::Broodwar->drawBoxMap(q.x * 32, q.y * 32, q.x * 32 + 4 * 32, q.y * 32 + 2 * 32, BWAPI::Colors::Orange);
		}

		//if this is an island expansion, draw a yellow circle around the base location
		if ((*i)->isIsland())
			BWAPI::Broodwar->drawCircleMap(c, 80, BWAPI::Colors::Yellow);
	}

	//we will iterate through all the regions and draw the polygon outline of it in green.
	for (std::set<BWTA::Region*>::const_iterator r = BWTA::getRegions().begin(); r != BWTA::getRegions().end(); r++)
	{
		BWTA::Polygon p = (*r)->getPolygon();
		for (int j = 0; j<(int)p.size(); j++)
		{
			BWAPI::Position point1 = p[j];
			BWAPI::Position point2 = p[(j + 1) % p.size()];
			BWAPI::Broodwar->drawLineMap(point1, point2, BWAPI::Colors::Green);
		}
	}

	//we will visualize the chokepoints with red lines
	for (std::set<BWTA::Region*>::const_iterator r = BWTA::getRegions().begin(); r != BWTA::getRegions().end(); r++)
	{
		for (std::set<BWTA::Chokepoint*>::const_iterator c = (*r)->getChokepoints().begin(); c != (*r)->getChokepoints().end(); c++)
		{
			BWAPI::Position point1 = (*c)->getSides().first;
			BWAPI::Position point2 = (*c)->getSides().second;
			BWAPI::Broodwar->drawLineMap(point1, point2, BWAPI::Colors::Red);
		}
	}
}

void InformationManager::onStart() {}

void InformationManager::updateUnit(BWAPI::UnitInterface* unit)
{
	if (unit->getPlayer() == BWAPI::Broodwar->enemy())
	{
		enemyUnitData.updateUnit(unit);
	}
	else if (unit->getPlayer() == BWAPI::Broodwar->self())
	{
		selfUnitData.updateUnit(unit);
	}
}

// is the unit valid?
bool InformationManager::isValidUnit(BWAPI::UnitInterface* unit) 
{
	// we only care about our units and enemy units
	if (unit->getPlayer() != BWAPI::Broodwar->self() && unit->getPlayer() != BWAPI::Broodwar->enemy()) 
	{
		return false;
	}

	// if it's a weird unit, don't bother
	if (unit->getType() == BWAPI::UnitTypes::None || unit->getType() == BWAPI::UnitTypes::Unknown ||
		unit->getType() == BWAPI::UnitTypes::Zerg_Larva || unit->getType() == BWAPI::UnitTypes::Zerg_Egg) 
	{
		return false;
	}

	// if the position isn't valid throw it out
	if (!unit->getPosition().isValid()) 
	{
		return false;	
	}

	// s'all good baby baby
	return true;
}

void InformationManager::onUnitDestroy(BWAPI::UnitInterface* unit) 
{ 
	// erase the unit
	if (unit->getPlayer() == BWAPI::Broodwar->enemy())
	{
		enemyUnitData.removeUnit(unit);
	}
	else if (unit->getPlayer() == BWAPI::Broodwar->self())
	{
		selfUnitData.removeUnit(unit);
	}
}

BWAPI::UnitInterface* InformationManager::getClosestUnitToTarget(BWAPI::UnitType type, BWAPI::Position target)
{
	BWAPI::UnitInterface* closestUnit = NULL;
	double closestDist = 100000;

	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (unit->getType() == type)
		{
			double dist = unit->getDistance(target);
			if (!closestUnit || dist < closestDist)
			{
				closestUnit = unit;
				closestDist = dist;
			}
		}
	}

	return closestUnit;
}

bool InformationManager::isCombatUnit(BWAPI::UnitType type) const
{
	if (Options::Modules::USING_COMBAT_PREDICTOR)
	{
		if (type == BWAPI::UnitTypes::Terran_SCV) //coz it can repair bunkers
			return true;

		if (type.canAttack() || type == BWAPI::UnitTypes::Terran_Medic || type == BWAPI::UnitTypes::Terran_Bunker)
		{
			return true;
		}

	}
	

	if (type == BWAPI::UnitTypes::Zerg_Lurker/* || type == BWAPI::UnitTypes::Protoss_Dark_Templar*/)
	{
		return false;
	}

	// no workers or buildings allowed
	if (type.isWorker())
	{
		return false;
	}

	// check for various types of combat units
	if (type.canAttack() || 
		type == BWAPI::UnitTypes::Terran_Medic || 
		type == BWAPI::UnitTypes::Protoss_Observer)
	{
		return true;
	}
		
	return false;
}

void InformationManager::getNearbyForce(std::vector<UnitInfo> & unitInfo, BWAPI::Position p, BWAPI::PlayerInterface * player, int radius) 
{
	bool hasBunker = false;
	// for each unit we know about for that player
	FOR_EACH_UIMAP_CONST(iter, getUnitData(player).getUnits())
	{
		const UnitInfo & ui(iter->second);

		// if it's a combat unit we care about
		// and it's finished! 
		if (isCombatUnit(ui.type) && ui.completed)
		{
			// determine its attack range
			int range = 0;
			if (ui.type.groundWeapon() != BWAPI::WeaponTypes::None)
			{
				range = ui.type.groundWeapon().maxRange() + 40;
			}

			if (Options::Modules::USING_COMBAT_PREDICTOR)
			{
				if (ui.type == BWAPI::UnitTypes::Terran_Bunker)
				{
					range = 160 + 40; //marines = 128, bunker = +32
					hasBunker = true;
				}
			}
			
			if (Options::Modules::USING_COMBAT_PREDICTOR)
			{
				if (ui.type == BWAPI::UnitTypes::Terran_SCV) //SPECIAL CASE
				{
					if (ui.lastPosition.getDistance(p) <= 200)
						unitInfo.push_back(ui);
				}

				// if it can attack into the radius we care about
				else if (ui.lastPosition.getDistance(p) <= (radius + range))
				{
					// add it to the vector
					unitInfo.push_back(ui);
				}
			}
		}
		else if (ui.type.isDetector() && ui.lastPosition.getDistance(p) <= (radius + 250))
			// add it to the vector
			unitInfo.push_back(ui);

	}

	if (Options::Modules::USING_COMBAT_PREDICTOR)
	if (!hasBunker)
	{
		//delete all SCVs
		for (int i = 0; i < unitInfo.size();)
		{
			if (unitInfo[i].type == BWAPI::UnitTypes::Terran_SCV)
				unitInfo.erase(unitInfo.begin() + i);
			else
				i++;
		}
	}
}

bool InformationManager::nearbyForceHasCloaked(BWAPI::Position p, BWAPI::PlayerInterface * player, int radius) 
{
	if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawCircleMap(p.x, p.y, radius, BWAPI::Colors::Red);

	FOR_EACH_UIMAP_CONST(iter, getUnitData(player).getUnits())
	{
		const UnitInfo & ui(iter->second);
		BWAPI::UnitType type(ui.type);

		// we don't care about workers
		if (type.isWorker())
		{
			continue;
		}

		int range = 0;
		if (type.groundWeapon() != BWAPI::WeaponTypes::None)
		{
			range = type.groundWeapon().maxRange() + 40;
		}

		// if it's outside the radius we don't care
		if (ui.lastPosition.getDistance(p) > (radius + range))
		{
			continue;
		}

		if (type == BWAPI::UnitTypes::Zerg_Lurker ||
				type == BWAPI::UnitTypes::Protoss_Dark_Templar ||
				type == BWAPI::UnitTypes::Terran_Wraith)
		{
			return true;
		}
	}

	return false;
}

int InformationManager::getNumUnits(BWAPI::UnitType t, BWAPI::PlayerInterface * player)
{
	return getUnitData(player).getNumUnits(t);
}

const UnitData & InformationManager::getUnitData(BWAPI::PlayerInterface * player) const
{
	return (player == BWAPI::Broodwar->self()) ? selfUnitData : enemyUnitData;
}

const UnitData & InformationManager::getUnitData(BWAPI::UnitInterface* unit) const
{
	return getUnitData(unit->getPlayer());
}

bool InformationManager::enemyHasCloakedUnits()
{
	return enemyUnitData.hasCloakedUnits();
}

bool InformationManager::enemyHasDetector()
{
	return enemyUnitData.hasDetectorUnits();
}

bool InformationManager::tileContainsUnit(BWAPI::TilePosition tile)
{
	return map.canBuildHere(tile);
}