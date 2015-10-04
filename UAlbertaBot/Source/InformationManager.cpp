#include "Common.h"
#include "InformationManager.h"

using namespace UAlbertaBot;

InformationManager::InformationManager()
    : _self(BWAPI::Broodwar->self())
    , _enemy(BWAPI::Broodwar->enemy())
{
	initializeRegionInformation();
}

InformationManager & InformationManager::Instance() 
{
	static InformationManager instance;
	return instance;
}

void InformationManager::update() 
{
	updateUnitInfo();
	updateBaseLocationInfo();
}

void InformationManager::updateUnitInfo() 
{
	for (auto & unit : BWAPI::Broodwar->enemy()->getUnits())
	{
		updateUnit(unit);
	}

	for (auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		updateUnit(unit);
	}

	// remove bad enemy units
	_unitData[_enemy].removeBadUnits();
	_unitData[_self].removeBadUnits();
}

void InformationManager::initializeRegionInformation() 
{
	// set initial pointers to null
	_mainBaseLocations[_self] = BWTA::getStartLocation(BWAPI::Broodwar->self());
	_mainBaseLocations[_enemy] = BWTA::getStartLocation(BWAPI::Broodwar->enemy());

	// push that region into our occupied vector
	updateOccupiedRegions(BWTA::getRegion(_mainBaseLocations[_self]->getTilePosition()), BWAPI::Broodwar->self());
}


void InformationManager::updateBaseLocationInfo() 
{
	_occupiedRegions[_self].clear();
	_occupiedRegions[_enemy].clear();
		
	// if we haven't found the enemy main base location yet
	if (!_mainBaseLocations[_enemy]) 
	{ 
		// how many start locations have we explored
		int exploredStartLocations = 0;
		bool baseFound = false;

		// an undexplored base location holder
		BWTA::BaseLocation * unexplored = nullptr;

		for (BWTA::BaseLocation * startLocation : BWTA::getStartLocations()) 
		{
			if (isEnemyBuildingInRegion(BWTA::getRegion(startLocation->getTilePosition()))) 
			{
                if (Config::Debug::DrawScoutInfo)
                {
				    BWAPI::Broodwar->printf("Enemy base found by seeing it");
                }

				baseFound = true;
				_mainBaseLocations[_enemy] = startLocation;
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
            if (Config::Debug::DrawScoutInfo)
            {
                BWAPI::Broodwar->printf("Enemy base found by process of elimination");
            }
			
			_mainBaseLocations[_enemy] = unexplored;
			updateOccupiedRegions(BWTA::getRegion(unexplored->getTilePosition()), BWAPI::Broodwar->enemy());
		}
	// otherwise we do know it, so push it back
	} 
	else 
	{
		updateOccupiedRegions(BWTA::getRegion(_mainBaseLocations[_enemy]->getTilePosition()), BWAPI::Broodwar->enemy());
	}

	// for each enemy unit we know about
	for (const auto & kv : _unitData[_enemy].getUnits())
	{
		const UnitInfo & ui(kv.second);
		BWAPI::UnitType type = ui.type;

		// if the unit is a building
		if (type.isBuilding()) 
		{
			// update the enemy occupied regions
			updateOccupiedRegions(BWTA::getRegion(BWAPI::TilePosition(ui.lastPosition)), BWAPI::Broodwar->enemy());
		}
	}

	// for each of our units
	for (const auto & kv : _unitData[_self].getUnits())
	{
		const UnitInfo & ui(kv.second);
		BWAPI::UnitType type = ui.type;

		// if the unit is a building
		if (type.isBuilding()) 
		{
			// update the enemy occupied regions
			updateOccupiedRegions(BWTA::getRegion(BWAPI::TilePosition(ui.lastPosition)), BWAPI::Broodwar->self());
		}
	}
}

void InformationManager::updateOccupiedRegions(BWTA::Region * region, BWAPI::Player player) 
{
	// if the region is valid (flying buildings may be in nullptr regions)
	if (region)
	{
		// add it to the list of occupied regions
		_occupiedRegions[player].insert(region);
	}
}

bool InformationManager::isEnemyBuildingInRegion(BWTA::Region * region) 
{
	// invalid regions aren't considered the same, but they will both be null
	if (!region)
	{
		return false;
	}

	for (const auto & kv : _unitData[_enemy].getUnits())
	{
		const UnitInfo & ui(kv.second);
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

const UIMap & InformationManager::getUnitInfo(BWAPI::Player player) const
{
	return getUnitData(player).getUnits();
}

std::set<BWTA::Region *> & InformationManager::getOccupiedRegions(BWAPI::Player player)
{
	return _occupiedRegions[player];
}

BWTA::BaseLocation * InformationManager::getMainBaseLocation(BWAPI::Player player) 
{
	return _mainBaseLocations[player];
}

void InformationManager::drawExtendedInterface()
{
    if (!Config::Debug::DrawUnitHealthBars)
    {
        return;
    }

    int verticalOffset = -10;

    // draw enemy units
    for (const auto & kv : getUnitData(BWAPI::Broodwar->enemy()).getUnits())
	{
        const UnitInfo & ui(kv.second);

		BWAPI::UnitType type(ui.type);
        int hitPoints = ui.lastHealth;
        int shields = ui.lastShields;

        const BWAPI::Position & pos = ui.lastPosition;

        int left    = pos.x - type.dimensionLeft();
        int right   = pos.x + type.dimensionRight();
        int top     = pos.y - type.dimensionUp();
        int bottom  = pos.y + type.dimensionDown();

        if (!BWAPI::Broodwar->isVisible(BWAPI::TilePosition(ui.lastPosition)))
        {
            BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, top), BWAPI::Position(right, bottom), BWAPI::Colors::Grey, false);
            BWAPI::Broodwar->drawTextMap(BWAPI::Position(left + 3, top + 4), "%s", ui.type.getName().c_str());
        }
        
        if (!type.isResourceContainer() && type.maxHitPoints() > 0)
        {
            double hpRatio = (double)hitPoints / (double)type.maxHitPoints();
        
            BWAPI::Color hpColor = BWAPI::Colors::Green;
            if (hpRatio < 0.66) hpColor = BWAPI::Colors::Orange;
            if (hpRatio < 0.33) hpColor = BWAPI::Colors::Red;

            int ratioRight = left + (int)((right-left) * hpRatio);
            int hpTop = top + verticalOffset;
            int hpBottom = top + 4 + verticalOffset;

            BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Grey, true);
            BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(ratioRight, hpBottom), hpColor, true);
            BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Black, false);

            int ticWidth = 3;

            for (int i(left); i < right-1; i+=ticWidth)
            {
                BWAPI::Broodwar->drawLineMap(BWAPI::Position(i, hpTop), BWAPI::Position(i, hpBottom), BWAPI::Colors::Black);
            }
        }

        if (!type.isResourceContainer() && type.maxShields() > 0)
        {
            double shieldRatio = (double)shields / (double)type.maxShields();
        
            int ratioRight = left + (int)((right-left) * shieldRatio);
            int hpTop = top - 3 + verticalOffset;
            int hpBottom = top + 1 + verticalOffset;

            BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Grey, true);
            BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(ratioRight, hpBottom), BWAPI::Colors::Blue, true);
            BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Black, false);

            int ticWidth = 3;

            for (int i(left); i < right-1; i+=ticWidth)
            {
                BWAPI::Broodwar->drawLineMap(BWAPI::Position(i, hpTop), BWAPI::Position(i, hpBottom), BWAPI::Colors::Black);
            }
        }

    }

    // draw neutral units and our units
    for (auto & unit : BWAPI::Broodwar->getAllUnits())
    {
        if (unit->getPlayer() == BWAPI::Broodwar->enemy())
        {
            continue;
        }

        const BWAPI::Position & pos = unit->getPosition();

        int left    = pos.x - unit->getType().dimensionLeft();
        int right   = pos.x + unit->getType().dimensionRight();
        int top     = pos.y - unit->getType().dimensionUp();
        int bottom  = pos.y + unit->getType().dimensionDown();

        //BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, top), BWAPI::Position(right, bottom), BWAPI::Colors::Grey, false);

        if (!unit->getType().isResourceContainer() && unit->getType().maxHitPoints() > 0)
        {
            double hpRatio = (double)unit->getHitPoints() / (double)unit->getType().maxHitPoints();
        
            BWAPI::Color hpColor = BWAPI::Colors::Green;
            if (hpRatio < 0.66) hpColor = BWAPI::Colors::Orange;
            if (hpRatio < 0.33) hpColor = BWAPI::Colors::Red;

            int ratioRight = left + (int)((right-left) * hpRatio);
            int hpTop = top + verticalOffset;
            int hpBottom = top + 4 + verticalOffset;

            BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Grey, true);
            BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(ratioRight, hpBottom), hpColor, true);
            BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Black, false);

            int ticWidth = 3;

            for (int i(left); i < right-1; i+=ticWidth)
            {
                BWAPI::Broodwar->drawLineMap(BWAPI::Position(i, hpTop), BWAPI::Position(i, hpBottom), BWAPI::Colors::Black);
            }
        }

        if (!unit->getType().isResourceContainer() && unit->getType().maxShields() > 0)
        {
            double shieldRatio = (double)unit->getShields() / (double)unit->getType().maxShields();
        
            int ratioRight = left + (int)((right-left) * shieldRatio);
            int hpTop = top - 3 + verticalOffset;
            int hpBottom = top + 1 + verticalOffset;

            BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Grey, true);
            BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(ratioRight, hpBottom), BWAPI::Colors::Blue, true);
            BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Black, false);

            int ticWidth = 3;

            for (int i(left); i < right-1; i+=ticWidth)
            {
                BWAPI::Broodwar->drawLineMap(BWAPI::Position(i, hpTop), BWAPI::Position(i, hpBottom), BWAPI::Colors::Black);
            }
        }

        if (unit->getType().isResourceContainer() && unit->getInitialResources() > 0)
        {
            
            double mineralRatio = (double)unit->getResources() / (double)unit->getInitialResources();
        
            int ratioRight = left + (int)((right-left) * mineralRatio);
            int hpTop = top + verticalOffset;
            int hpBottom = top + 4 + verticalOffset;

            BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Grey, true);
            BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(ratioRight, hpBottom), BWAPI::Colors::Cyan, true);
            BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Black, false);

            int ticWidth = 3;

            for (int i(left); i < right-1; i+=ticWidth)
            {
                BWAPI::Broodwar->drawLineMap(BWAPI::Position(i, hpTop), BWAPI::Position(i, hpBottom), BWAPI::Colors::Black);
            }
        }
    }
}

void InformationManager::drawUnitInformation(int x, int y) 
{
	if (!Config::Debug::DrawEnemyUnitInfo)
    {
        return;
    }

	std::string prefix = "\x04";

	BWAPI::Broodwar->drawTextScreen(x, y-10, "\x03 Self Loss:\x04 Minerals: \x1f%d \x04Gas: \x07%d", _unitData[_self].getMineralsLost(), _unitData[_self].getGasLost());
    BWAPI::Broodwar->drawTextScreen(x, y, "\x03 Enemy Loss:\x04 Minerals: \x1f%d \x04Gas: \x07%d", _unitData[_enemy].getMineralsLost(), _unitData[_enemy].getGasLost());
	BWAPI::Broodwar->drawTextScreen(x, y+10, "\x04 Enemy: %s", BWAPI::Broodwar->enemy()->getName().c_str());
	BWAPI::Broodwar->drawTextScreen(x, y+20, "\x04 UNIT NAME");
	BWAPI::Broodwar->drawTextScreen(x+140, y+20, "\x04#");
	BWAPI::Broodwar->drawTextScreen(x+160, y+20, "\x04X");

	int yspace = 0;

	// for each unit in the queue
	for (BWAPI::UnitType t : BWAPI::UnitTypes::allUnitTypes()) 
	{
		int numUnits = _unitData[_enemy].getNumUnits(t);
		int numDeadUnits = _unitData[_enemy].getNumDeadUnits(t);

		// if there exist units in the vector
		if (numUnits > 0) 
		{
			if (t.isDetector())			{ prefix = "\x10"; }		
			else if (t.canAttack())		{ prefix = "\x08"; }		
			else if (t.isBuilding())	{ prefix = "\x03"; }
			else						{ prefix = "\x04"; }

			BWAPI::Broodwar->drawTextScreen(x, y+40+((yspace)*10), " %s%s", prefix.c_str(), t.getName().c_str());
			BWAPI::Broodwar->drawTextScreen(x+140, y+40+((yspace)*10), "%s%d", prefix.c_str(), numUnits);
			BWAPI::Broodwar->drawTextScreen(x+160, y+40+((yspace++)*10), "%s%d", prefix.c_str(), numDeadUnits);
		}
	}
}

void InformationManager::drawMapInformation()
{
    if (!Config::Debug::DrawBWTAInfo)
    {
        return;
    }

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

void InformationManager::updateUnit(BWAPI::Unit unit)
{
    if (!(unit->getPlayer() == _self || unit->getPlayer() == _enemy))
    {
        return;
    }

    _unitData[unit->getPlayer()].updateUnit(unit);
}

// is the unit valid?
bool InformationManager::isValidUnit(BWAPI::Unit unit) 
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

void InformationManager::onUnitDestroy(BWAPI::Unit unit) 
{ 
    if (unit->getType().isNeutral())
    {
        return;
    }

    _unitData[unit->getPlayer()].removeUnit(unit);
}

bool InformationManager::isCombatUnit(BWAPI::UnitType type) const
{
	if (type == BWAPI::UnitTypes::Zerg_Lurker/* || type == BWAPI::UnitTypes::Protoss_Dark_Templar*/)
	{
		return false;
	}

	// check for various types of combat units
	if (type.canAttack() || 
		type == BWAPI::UnitTypes::Terran_Medic || 
		type == BWAPI::UnitTypes::Protoss_Observer ||
        type == BWAPI::UnitTypes::Terran_Bunker)
	{
		return true;
	}
		
	return false;
}

void InformationManager::getNearbyForce(std::vector<UnitInfo> & unitInfo, BWAPI::Position p, BWAPI::Player player, int radius) 
{
	bool hasBunker = false;
	// for each unit we know about for that player
	for (const auto & kv : getUnitData(player).getUnits())
	{
		const UnitInfo & ui(kv.second);

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

			// if it can attack into the radius we care about
			if (ui.lastPosition.getDistance(p) <= (radius + range))
			{
				// add it to the vector
				unitInfo.push_back(ui);
			}
		}
		else if (ui.type.isDetector() && ui.lastPosition.getDistance(p) <= (radius + 250))
        {
			// add it to the vector
			unitInfo.push_back(ui);
        }
	}
}

int InformationManager::getNumUnits(BWAPI::UnitType t, BWAPI::Player player)
{
	return getUnitData(player).getNumUnits(t);
}

const UnitData & InformationManager::getUnitData(BWAPI::Player player) const
{
    return _unitData.find(player)->second;
}

bool InformationManager::enemyHasCloakedUnits()
{
    for (const auto & kv : getUnitData(_enemy).getUnits())
	{
		const UnitInfo & ui(kv.second);

        if (ui.type.isCloakable())
        {
            return true;
        }

        // assume they're going dts
        if (ui.type == BWAPI::UnitTypes::Protoss_Citadel_of_Adun)
        {
            return true;
        }

        if (ui.type == BWAPI::UnitTypes::Protoss_Observatory)
        {
            return true;
        }
    }

	return false;
}
