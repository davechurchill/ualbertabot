#include "MicroManager.h"

using namespace UAlbertaBot;

MicroManager::MicroManager() 
{
}

void MicroManager::setUnits(const BWAPI::Unitset & u) 
{ 
	_units = u; 
}

BWAPI::Position MicroManager::calcCenter() const
{
    if (_units.empty())
    {
        if (Config::Debug::DrawSquadInfo)
        {
            BWAPI::Broodwar->printf("Squad::calcCenter() called on empty squad");
        }
        return BWAPI::Position(0,0);
    }

	BWAPI::Position accum(0,0);
	for (auto & unit : _units)
	{
		accum += unit->getPosition();
	}
	return BWAPI::Position(accum.x / _units.size(), accum.y / _units.size());
}

void MicroManager::execute(const SquadOrder & inputOrder)
{
	// Nothing to do if we have no units
	if (_units.empty() || !(inputOrder.getType() == SquadOrderTypes::Attack || inputOrder.getType() == SquadOrderTypes::Defend))
	{
		return;
	}

	order = inputOrder;
	drawOrderText();

	// Discover enemies within region of interest
	BWAPI::Unitset nearbyEnemies;

	// if the order is to defend, we only care about units in the radius of the defense
	if (order.getType() == SquadOrderTypes::Defend)
	{
		MapGrid::Instance().GetUnits(nearbyEnemies, order.getPosition(), order.getRadius(), false, true);
	
	} // otherwise we want to see everything on the way
	else if (order.getType() == SquadOrderTypes::Attack) 
	{
		MapGrid::Instance().GetUnits(nearbyEnemies, order.getPosition(), order.getRadius(), false, true);
		for (auto & unit : _units) 
		{
			BWAPI::Unit u = unit;
			BWAPI::UnitType t = u->getType();
			MapGrid::Instance().GetUnits(nearbyEnemies, unit->getPosition(), order.getRadius(), false, true);
		}
	}

	// the following block of code attacks all units on the way to the order position
	// we want to do this if the order is attack, defend, or harass
	if (order.getType() == SquadOrderTypes::Attack || order.getType() == SquadOrderTypes::Defend) 
	{
        // if this is a worker defense force
        if (_units.size() == 1 && (*_units.begin())->getType().isWorker())
        {
            executeMicro(nearbyEnemies);
        }
        // otherwise it is a normal attack force
        else
        {
            // if this is a defense squad then we care about all units in the area
            if (order.getType() == SquadOrderTypes::Defend)
            {
                executeMicro(nearbyEnemies);
            }
            // otherwise we only care about workers if they are in their own region
            else
            {
                 // if this is the an attack squad
                BWAPI::Unitset workersRemoved;

                for (auto & enemyUnit : nearbyEnemies) 
		        {
                    // if its not a worker add it to the targets
			        if (!enemyUnit->getType().isWorker())
                    {
                        workersRemoved.insert(enemyUnit);
                    }
                    // if it is a worker
                    else
                    {
                        for (BWTA::Region * enemyRegion : InformationManager::Instance().getOccupiedRegions(BWAPI::Broodwar->enemy()))
                        {
                            // only add it if it's in their region
                            if (BWTA::getRegion(BWAPI::TilePosition(enemyUnit->getPosition())) == enemyRegion)
                            {
                                workersRemoved.insert(enemyUnit);
                            }
                        }
                    }
		        }

		        // Allow micromanager to handle enemies
		        executeMicro(workersRemoved);
            }
        }
	}	
}

const BWAPI::Unitset & MicroManager::getUnits() const 
{ 
    return _units; 
}

void MicroManager::regroup(const BWAPI::Position & regroupPosition) const
{
    BWAPI::Position ourBasePosition = BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation());
    int regroupDistanceFromBase = MapTools::Instance().getGroundDistance(regroupPosition, ourBasePosition);

	// for each of the units we have
	for (auto & unit : _units)
	{
        int unitDistanceFromBase = MapTools::Instance().getGroundDistance(unit->getPosition(), ourBasePosition);

		// if the unit is outside the regroup area
        if (unitDistanceFromBase > regroupDistanceFromBase)
        {
            Micro::SmartMove(unit, ourBasePosition);
        }
		else if (unit->getDistance(regroupPosition) > 100)
		{
			// regroup it
			Micro::SmartMove(unit, regroupPosition);
		}
		else
		{
			Micro::SmartAttackMove(unit, unit->getPosition());
		}
	}
}

bool MicroManager::unitNearEnemy(BWAPI::Unit unit)
{
	assert(unit);

	BWAPI::Unitset enemyNear;

	MapGrid::Instance().GetUnits(enemyNear, unit->getPosition(), 800, false, true);

	return enemyNear.size() > 0;
}

// returns true if position:
// a) is walkable
// b) doesn't have buildings on it
// c) doesn't have a unit on it that can attack ground
bool MicroManager::checkPositionWalkable(BWAPI::Position pos) 
{
	// get x and y from the position
	int x(pos.x), y(pos.y);

	// walkable tiles exist every 8 pixels
	bool good = BWAPI::Broodwar->isWalkable(x/8, y/8);
	
	// if it's not walkable throw it out
	if (!good) return false;
	
	// for each of those units, if it's a building or an attacking enemy unit we don't want to go there
	for (auto & unit : BWAPI::Broodwar->getUnitsOnTile(x/32, y/32)) 
	{
		if	(unit->getType().isBuilding() || unit->getType().isResourceContainer() || 
			(unit->getPlayer() != BWAPI::Broodwar->self() && unit->getType().groundWeapon() != BWAPI::WeaponTypes::None)) 
		{		
				return false;
		}
	}

	// otherwise it's okay
	return true;
}

void MicroManager::trainSubUnits(BWAPI::Unit unit) const
{
	if (unit->getType() == BWAPI::UnitTypes::Protoss_Reaver)
	{
		unit->train(BWAPI::UnitTypes::Protoss_Scarab);
	}
	else if (unit->getType() == BWAPI::UnitTypes::Protoss_Carrier)
	{
		unit->train(BWAPI::UnitTypes::Protoss_Interceptor);
	}
}

bool MicroManager::unitNearChokepoint(BWAPI::Unit unit) const
{
	for (BWTA::Chokepoint * choke : BWTA::getChokepoints())
	{
		if (unit->getDistance(choke->getCenter()) < 80)
		{
			return true;
		}
	}

	return false;
}

void MicroManager::drawOrderText() 
{
	for (auto & unit : _units) 
    {
		if (Config::Debug::DrawUnitTargetInfo) BWAPI::Broodwar->drawTextMap(unit->getPosition().x, unit->getPosition().y, "%s", order.getStatus().c_str());
	}
}
