#include "MicroManager.h"
#include "Global.h"
#include "Micro.h"

using namespace UAlbertaBot;

MicroManager::MicroManager(const AKBot::OpponentView& opponentView, const BaseLocationManager& bases)
	: bases(bases)
	, opponentView(opponentView)
{
}

void MicroManager::setUnits(const std::vector<BWAPI::Unit> & u) 
{ 
	_units = u; 
}

void MicroManager::execute(const MapTools & map, const SquadOrder & inputOrder, int currentFrame)
{
	// Nothing to do if we have no units
	if (_units.empty() || !(inputOrder.getType() == SquadOrderTypes::Attack || inputOrder.getType() == SquadOrderTypes::Defend))
	{
		return;
	}

	order = inputOrder;

	// Discover enemies within region of interest
	std::vector<BWAPI::Unit> nearbyEnemies;

	// if the order is to defend, we only care about units in the radius of the defense
	if (order.getType() == SquadOrderTypes::Defend)
	{
		map.GetUnitsInRadius(nearbyEnemies, order.getPosition(), order.getRadius(), false, true);
	
	} // otherwise we want to see everything on the way
	else if (order.getType() == SquadOrderTypes::Attack) 
	{
		map.GetUnitsInRadius(nearbyEnemies, order.getPosition(), order.getRadius(), false, true);
		for (auto & unit : _units) 
		{
			BWAPI::Unit u = unit;
			BWAPI::UnitType t = u->getType();
			map.GetUnitsInRadius(nearbyEnemies, unit->getPosition(), order.getRadius(), false, true);
		}
	}

	// the following block of code attacks all units on the way to the order position
	// we want to do this if the order is attack, defend, or harass
	if (order.getType() == SquadOrderTypes::Attack || order.getType() == SquadOrderTypes::Defend) 
	{
        // if this is a worker defense force
        if (_units.size() == 1 && (*_units.begin())->getType().isWorker())
        {
            executeMicro(nearbyEnemies, currentFrame);
        }
        // otherwise it is a normal attack force
        else
        {
            // if this is a defense squad then we care about all units in the area
            if (order.getType() == SquadOrderTypes::Defend)
            {
                executeMicro(nearbyEnemies, currentFrame);
            }
            // otherwise we only care about workers if they are in their own region
            else
            {
                 // if this is the an attack squad
                std::vector<BWAPI::Unit> workersRemoved;

                for (auto & enemyUnit : nearbyEnemies) 
		        {
                    // if its not a worker add it to the targets
			        if (!enemyUnit->getType().isWorker())
                    {
                        workersRemoved.push_back(enemyUnit);
                    }
                    // if it is a worker
                    else
                    {
						for (const auto& enemyPlayer : opponentView.enemies())
						{
							for (auto enemyBaseLocation : bases.getOccupiedBaseLocations(enemyPlayer))
							{
								// only add it if it's in their region
								if (enemyBaseLocation->containsPosition(enemyUnit->getPosition()))
								{
									workersRemoved.push_back(enemyUnit);
								}
							}
						}
                    }
		        }

		        // Allow micromanager to handle enemies
		        executeMicro(workersRemoved, currentFrame);
            }
        }
	}	
}

const std::vector<BWAPI::Unit> & MicroManager::getUnits() const 
{ 
    return _units; 
}

void MicroManager::regroup(const MapTools & map, const BWAPI::Position & regroupPosition, int currentFrame) const
{
    BWAPI::Position ourBasePosition = BWAPI::Position(opponentView.self()->getStartLocation());
    int regroupDistanceFromBase = map.getGroundDistance(regroupPosition, ourBasePosition);

	// for each of the units we have
	for (auto & unit : _units)
	{
        int unitDistanceFromBase = map.getGroundDistance(unit->getPosition(), ourBasePosition);

		// if the unit is outside the regroup area
        if (unitDistanceFromBase > regroupDistanceFromBase)
        {
            Micro::SmartMove(unit, ourBasePosition, currentFrame);
        }
		else if (unit->getDistance(regroupPosition) > 100)
		{
			// regroup it
			Micro::SmartMove(unit, regroupPosition, currentFrame);
		}
		else
		{
			Micro::SmartAttackMove(unit, unit->getPosition(), currentFrame);
		}
	}
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
			(unit->getPlayer() != opponentView.self() && unit->getType().groundWeapon() != BWAPI::WeaponTypes::None))
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
