#include "MicroManager.h"

using namespace UAlbertaBot;

void MicroManager::setUnits(const std::vector<BWAPI::UnitInterface *> & u) 
{ 
	units = u; 
}

BWAPI::Position MicroManager::calcCenter() const
{
    if (units.empty())
    {
        if (Config::Debug::DrawSquadInfo)
        {
            BWAPI::Broodwar->printf("Squad::calcCenter() called on empty squad");
        }
        return BWAPI::Position(0,0);
    }
	assert(!units.empty());

	BWAPI::Position accum(0,0);
	for (BWAPI::UnitInterface* unit : units)
	{
		accum += unit->getPosition();
	}
	return BWAPI::Position(accum.x / units.size(), accum.y / units.size());
}

void MicroManager::execute(const SquadOrder & inputOrder)
{
	// Nothing to do if we have no units
	if (units.empty() || !(inputOrder.getType() == SquadOrderTypes::Attack || inputOrder.getType() == SquadOrderTypes::Defend))
	{
		return;
	}

	order = inputOrder;
	drawOrderText();

	// Discover enemies within region of interest
	std::vector<BWAPI::UnitInterface *> nearbyEnemies;

	// if the order is to defend, we only care about units in the radius of the defense
	if (order.getType() == SquadOrderTypes::Defend)
	{
		MapGrid::Instance().GetUnits(nearbyEnemies, order.getPosition(), 800, false, true);
	
	} // otherwise we want to see everything on the way
	else if (order.getType() == SquadOrderTypes::Attack) 
	{
		MapGrid::Instance().GetUnits(nearbyEnemies, order.getPosition(), 800, false, true);
		for (BWAPI::UnitInterface* unit : units) 
		{
			BWAPI::UnitInterface* u = unit;
			BWAPI::UnitType t = u->getType();
			MapGrid::Instance().GetUnits(nearbyEnemies, unit->getPosition(), 800, false, true);
		}
	}

	// the following block of code attacks all units on the way to the order position
	// we want to do this if the order is attack, defend, or harass
	if (order.getType() == SquadOrderTypes::Attack || order.getType() == SquadOrderTypes::Defend) 
	{
        // if this is a worker defense force
        if (units.size() == 1 && units[0]->getType().isWorker())
        {
            executeMicro(nearbyEnemies);
        }
        // otherwise it is a normal attack force
        else
        {
             // remove enemy worker units unless they are in one of their occupied regions
            std::vector<BWAPI::UnitInterface *> workersRemoved;

            for (BWAPI::UnitInterface* enemyUnit : nearbyEnemies) 
		    {
                // if its not a worker add it to the targets
			    if (!enemyUnit->getType().isWorker())
                {
                    workersRemoved.push_back(enemyUnit);
                }
                // if it is a worker
                else
                {
                    for (BWTA::Region * enemyRegion : InformationManager::Instance().getOccupiedRegions(BWAPI::Broodwar->enemy()))
                    {
                        // only add it if it's in their region
                        if (BWTA::getRegion(BWAPI::TilePosition(enemyUnit->getPosition())) == enemyRegion)
                        {
                            workersRemoved.push_back(enemyUnit);
                        }
                    }
                }
		    }

		    // Allow micromanager to handle enemies
		    executeMicro(workersRemoved);
        }
	}	
}

void MicroManager::regroup(const BWAPI::Position & regroupPosition) const
{
	// for each of the units we have
	for (BWAPI::UnitInterface* unit : units)
	{
		// if the unit is outside the regroup area
		if (unit->getDistance(regroupPosition) > 100)
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

bool MicroManager::unitNearEnemy(BWAPI::UnitInterface* unit)
{
	assert(unit);

	std::vector<BWAPI::UnitInterface *> enemyNear;

	MapGrid::Instance().GetUnits(enemyNear, unit->getPosition(), 800, false, true);

	return enemyNear.size() > 0;
}

// returns true if position:
// a) is walkable
// b) doesn't have buildings on it
// c) doesn't have a unit on it that can attack ground
bool MicroManager::checkPositionWalkable(BWAPI::Position pos) {

	// get x and y from the position
	int x(pos.x), y(pos.y);

	// walkable tiles exist every 8 pixels
	bool good = BWAPI::Broodwar->isWalkable(x/8, y/8);
	
	// if it's not walkable throw it out
	if (!good) return false;
	
	// for each of those units, if it's a building or an attacking enemy unit we don't want to go there
	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->getUnitsOnTile(x/32, y/32)) 
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

void MicroManager::trainSubUnits(BWAPI::UnitInterface* unit) const
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

bool MicroManager::unitNearChokepoint(BWAPI::UnitInterface* unit) const
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

void MicroManager::drawOrderText() {

	for (BWAPI::UnitInterface* unit : units) 
    {
		if (Config::Debug::DrawUnitTargetInfo) BWAPI::Broodwar->drawTextMap(unit->getPosition().x, unit->getPosition().y, "%s", order.getStatus().c_str());
	}
}
