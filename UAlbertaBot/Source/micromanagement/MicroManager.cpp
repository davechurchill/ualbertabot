#include "MicroManager.h"

using namespace UAlbertaBot;

void MicroManager::setUnits(const UnitVector & u) 
{ 
	units = u; 
}

BWAPI::Position MicroManager::calcCenter() const
{
    if (units.empty())
    {
        if (Options::Debug::DRAW_UALBERTABOT_DEBUG)
        {
            BWAPI::Broodwar->printf("Squad::calcCenter() called on empty squad");
        }
        return BWAPI::Position(0,0);
    }
	assert(!units.empty());

	BWAPI::Position accum(0,0);
	BOOST_FOREACH(BWAPI::Unit * unit, units)
	{
		accum += unit->getPosition();
	}
	return BWAPI::Position(accum.x() / units.size(), accum.y() / units.size());
}

void MicroManager::execute(const SquadOrder & inputOrder)
{
	// Nothing to do if we have no units
	if(units.empty() || !(inputOrder.type == SquadOrder::Attack || inputOrder.type == SquadOrder::Defend))
	{
		//BWAPI::Broodwar->printf("Gots no units, fix shit up (%d)", order.type);
		return;
	}
	order = inputOrder;
	drawOrderText();

	// Discover enemies within region of interest
	UnitVector nearbyEnemies;

	// if the order is to defend, we only care about units in the radius of the defense
	if (order.type == order.Defend)
	{
		MapGrid::Instance().GetUnits(nearbyEnemies, order.position, 800, false, true);
	
	} // otherwise we want to see everything on the way
	else if (order.type == order.Attack) 
	{
		MapGrid::Instance().GetUnits(nearbyEnemies, order.position, 800, false, true);
		BOOST_FOREACH (BWAPI::Unit * unit, units) 
		{
			BWAPI::Unit * u = unit;
			BWAPI::UnitType t = u->getType();
			MapGrid::Instance().GetUnits(nearbyEnemies, unit->getPosition(), 800, false, true);
		}
	}

	// the following block of code attacks all units on the way to the order position
	// we want to do this if the order is attack, defend, or harass
	if (order.type == order.Attack || order.type == order.Defend) 
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
            UnitVector workersRemoved;

            BOOST_FOREACH (BWAPI::Unit * enemyUnit, nearbyEnemies) 
		    {
                // if its not a worker add it to the targets
			    if (!enemyUnit->getType().isWorker())
                {
                    workersRemoved.push_back(enemyUnit);
                }
                // if it is a worker
                else
                {
                    BOOST_FOREACH(BWTA::Region * enemyRegion, InformationManager::Instance().getOccupiedRegions(BWAPI::Broodwar->enemy()))
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
	BOOST_FOREACH (BWAPI::Unit * unit, units)
	{
		// if the unit is outside the regroup area
		if (unit->getDistance(regroupPosition) > 100)
		{
			// regroup it
			BWAPI::Broodwar->drawCircleMap(unit->getPosition().x(), unit->getPosition().y(), 20, BWAPI::Colors::Yellow);
			smartMove(unit, regroupPosition);
		}
		else
		{
			smartAttackMove(unit, unit->getPosition());
		}
	}
}

bool MicroManager::unitNearEnemy(BWAPI::Unit * unit)
{
	assert(unit);

	UnitVector enemyNear;

	MapGrid::Instance().GetUnits(enemyNear, unit->getPosition(), 800, false, true);

	return enemyNear.size() > 0;
}

// returns true if position:
// a) is walkable
// b) doesn't have buildings on it
// c) doesn't have a unit on it that can attack ground
bool MicroManager::checkPositionWalkable(BWAPI::Position pos) {

	// get x and y from the position
	int x(pos.x()), y(pos.y());

	// walkable tiles exist every 8 pixels
	bool good = BWAPI::Broodwar->isWalkable(x/8, y/8);
	
	// if it's not walkable throw it out
	if (!good) return false;
	
	// for each of those units, if it's a building or an attacking enemy unit we don't want to go there
	BOOST_FOREACH (BWAPI::Unit * unit, BWAPI::Broodwar->getUnitsOnTile(x/32, y/32)) 
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

void MicroManager::smartAttackUnit(BWAPI::Unit * attacker, BWAPI::Unit * target) const
{
	assert(attacker && target);

	// if we have issued a command to this unit already this frame, ignore this one
	if (attacker->getLastCommandFrame() >= BWAPI::Broodwar->getFrameCount() || attacker->isAttackFrame())
	{
		return;
	}

	// get the unit's current command
	BWAPI::UnitCommand currentCommand(attacker->getLastCommand());

	// if we've already told this unit to attack this target, ignore this command
	if (currentCommand.getType() == BWAPI::UnitCommandTypes::Attack_Unit &&	currentCommand.getTarget() == target)
	{
		return;
	}

	// if nothing prevents it, attack the target
	attacker->attack(target);

	if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawLineMap(	attacker->getPosition().x(), attacker->getPosition().y(),
									target->getPosition().x(), target->getPosition().y(),
									BWAPI::Colors::Red );

}

void MicroManager::smartAttackMove(BWAPI::Unit * attacker, BWAPI::Position targetPosition) const
{
	assert(attacker);

	// if we have issued a command to this unit already this frame, ignore this one
	if (attacker->getLastCommandFrame() >= BWAPI::Broodwar->getFrameCount() || attacker->isAttackFrame())
	{
		return;
	}

	// get the unit's current command
	BWAPI::UnitCommand currentCommand(attacker->getLastCommand());

	// if we've already told this unit to attack this target, ignore this command
	if (currentCommand.getType() == BWAPI::UnitCommandTypes::Attack_Move &&	currentCommand.getTargetPosition() == targetPosition)
	{
		return;
	}

	// if nothing prevents it, attack the target
	attacker->attack(targetPosition);

	if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawLineMap(	attacker->getPosition().x(), attacker->getPosition().y(),
									targetPosition.x(), targetPosition.y(),
									BWAPI::Colors::Orange );
}

void MicroManager::smartMove(BWAPI::Unit * attacker, BWAPI::Position targetPosition) const
{
	assert(attacker);

	// if we have issued a command to this unit already this frame, ignore this one
	if (attacker->getLastCommandFrame() >= BWAPI::Broodwar->getFrameCount() || attacker->isAttackFrame())
	{
		if (attacker->isSelected())
		{
			BWAPI::Broodwar->printf("Attack Frame");
		}
		return;
	}

	// get the unit's current command
	BWAPI::UnitCommand currentCommand(attacker->getLastCommand());

	// if we've already told this unit to attack this target, ignore this command
	if (   (currentCommand.getType() == BWAPI::UnitCommandTypes::Move)
		&& (currentCommand.getTargetPosition() == targetPosition) 
		&& (BWAPI::Broodwar->getFrameCount() - attacker->getLastCommandFrame() < 5)
		&& attacker->isMoving())
	{
		if (attacker->isSelected())
		{
			BWAPI::Broodwar->printf("Previous Command Frame=%d Pos=(%d, %d)", attacker->getLastCommandFrame(), currentCommand.getTargetPosition().x(), currentCommand.getTargetPosition().y());
		}
		return;
	}

	// if nothing prevents it, attack the target
	attacker->move(targetPosition);

	if (Options::Debug::DRAW_UALBERTABOT_DEBUG) 
	{
		BWAPI::Broodwar->drawLineMap(attacker->getPosition().x(), attacker->getPosition().y(),
									 targetPosition.x(), targetPosition.y(), BWAPI::Colors::Orange);
	}
}

void MicroManager::trainSubUnits(BWAPI::Unit * unit) const
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

bool MicroManager::unitNearChokepoint(BWAPI::Unit * unit) const
{
	BOOST_FOREACH (BWTA::Chokepoint * choke, BWTA::getChokepoints())
	{
		if (unit->getDistance(choke->getCenter()) < 80)
		{
			return true;
		}
	}

	return false;
}

void MicroManager::drawOrderText() {

	BOOST_FOREACH (BWAPI::Unit * unit, units) {
		if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextMap(unit->getPosition().x(), unit->getPosition().y(), "%s", order.getStatus().c_str());
	}
}
