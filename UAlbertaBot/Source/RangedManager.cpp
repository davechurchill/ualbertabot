#include "RangedManager.h"
#include "UnitUtil.h"
#include <BWAPI/Game.h>
#include <BWAPI/Player.h>
#include "Micro.h"

using namespace UAlbertaBot;

RangedManager::RangedManager(
	shared_ptr<AKBot::OpponentView> opponentView,
	shared_ptr<BaseLocationManager> bases,
	const BotMicroConfiguration& microConfiguration)
	: MicroManager(opponentView, bases)
	, _microConfiguration(microConfiguration)
{ 
}

void RangedManager::executeMicro(const std::vector<BWAPI::Unit> & targets, int currentFrame)
{
	assignTargets(targets, currentFrame);
}


void RangedManager::assignTargets(const std::vector<BWAPI::Unit> & targets, int currentFrame)
{
    const std::vector<BWAPI::Unit> & rangedUnits = getUnits();

	// figure out targets
	std::vector<BWAPI::Unit> rangedUnitTargets;
    std::copy_if(targets.begin(), targets.end(), std::inserter(rangedUnitTargets, rangedUnitTargets.end()), [](BWAPI::Unit u){ return u->isVisible(); });

    for (auto & rangedUnit : rangedUnits)
	{
		// train sub units such as scarabs or interceptors
		//trainSubUnits(rangedUnit);

		// if the order is to attack or defend
		if (order.getType() == SquadOrderTypes::Attack || order.getType() == SquadOrderTypes::Defend) 
        {
			// if there are targets
			if (!rangedUnitTargets.empty())
			{
				// find the best target for this zealot
				BWAPI::Unit target = getTarget(rangedUnit, rangedUnitTargets);

				// Code below now moved to CombatCommanderDebug
				// I don't know how target variable and rangedUnit->getTargetPosition() related right now
                /*if (target && _microConfiguration.DrawUnitTargetInfo) 
	            {
		            canvas.drawLineMap(rangedUnit->getPosition(), rangedUnit->getTargetPosition(), BWAPI::Colors::Purple);
	            }*/

				// attack it
                if (_microConfiguration.KiteWithRangedUnits)
                {
                    if (rangedUnit->getType() == BWAPI::UnitTypes::Zerg_Mutalisk || rangedUnit->getType() == BWAPI::UnitTypes::Terran_Vulture)
                    {
				        Micro::MutaDanceTarget(rangedUnit, target);
                    }
                    else
                    {
                        Micro::SmartKiteTarget(rangedUnit, target, currentFrame, _microConfiguration.KiteLongerRangedUnits);
                    }
                }
                else
                {
                    Micro::SmartAttackUnit(rangedUnit, target, currentFrame);
                }
			}
			// if there are no targets
			else
			{
				// if we're not near the order position
				if (rangedUnit->getDistance(order.getPosition()) > 100)
				{
					// move to it
					Micro::SmartAttackMove(rangedUnit, order.getPosition(), currentFrame);
				}
			}
		}
	}
}

// get a target for the zealot to attack
BWAPI::Unit RangedManager::getTarget(BWAPI::Unit rangedUnit, const std::vector<BWAPI::Unit> & targets)
{
	int bestPriorityDistance = 1000000;
    int bestPriority = 0;
    
    double bestLTD = 0;

    int highPriority = 0;
	double closestDist = std::numeric_limits<double>::infinity();
	BWAPI::Unit closestTarget = nullptr;

    for (const auto & target : targets)
    {
        double distance         = rangedUnit->getDistance(target);
        double LTD              = UnitUtil::CalculateLTD(target, rangedUnit);
        int priority            = getAttackPriority(rangedUnit, target);
        bool targetIsThreat     = LTD > 0;
        
		if (!closestTarget || (priority > highPriority) || (priority == highPriority && distance < closestDist))
		{
			closestDist = distance;
			highPriority = priority;
			closestTarget = target;
		}       
    }

    return closestTarget;
}

	// get the attack priority of a type in relation to a zergling
int RangedManager::getAttackPriority(BWAPI::Unit rangedUnit, BWAPI::Unit target) 
{
	BWAPI::UnitType rangedType = rangedUnit->getType();
	BWAPI::UnitType targetType = target->getType();

    if (rangedUnit->getType() == BWAPI::UnitTypes::Zerg_Scourge)
    {
        if (target->getType() == BWAPI::UnitTypes::Protoss_Carrier)
        {
            
            return 100;
        }

        if (target->getType() == BWAPI::UnitTypes::Protoss_Corsair)
        {
            return 90;
        }
    }

	bool isThreat = rangedType.isFlyer() ? targetType.airWeapon() != BWAPI::WeaponTypes::None : targetType.groundWeapon() != BWAPI::WeaponTypes::None;

    if (target->getType().isWorker())
    {
        isThreat = false;
    }

    if (target->getType() == BWAPI::UnitTypes::Zerg_Larva || target->getType() == BWAPI::UnitTypes::Zerg_Egg)
    {
        return 0;
    }

    if (rangedUnit->isFlying() && target->getType() == BWAPI::UnitTypes::Protoss_Carrier)
    {
        return 101;
    }

    // if the target is building something near our base something is fishy
    BWAPI::Position ourBasePosition = BWAPI::Position(opponentView->self()->getStartLocation());
    if (target->getType().isWorker() && (target->isConstructing() || target->isRepairing()) && target->getDistance(ourBasePosition) < 1200)
    {
        return 100;
    }

    if (target->getType().isBuilding() && (target->isCompleted() || target->isBeingConstructed()) && target->getDistance(ourBasePosition) < 1200)
    {
        return 90;
    }
    
	// highest priority is something that can attack us or aid in combat
    if (targetType ==  BWAPI::UnitTypes::Terran_Bunker || isThreat)
    {
        return 11;
    }
	// next priority is worker
	else if (targetType.isWorker()) 
	{
        if (rangedUnit->getType() == BWAPI::UnitTypes::Terran_Vulture)
        {
            return 11;
        }

  		return 11;
	}
    // next is special buildings
	else if (targetType == BWAPI::UnitTypes::Zerg_Spawning_Pool)
	{
		return 5;
	}
	// next is special buildings
	else if (targetType == BWAPI::UnitTypes::Protoss_Pylon)
	{
		return 5;
	}
	// next is buildings that cost gas
	else if (targetType.gasPrice() > 0)
	{
		return 4;
	}
	else if (targetType.mineralPrice() > 0)
	{
		return 3;
	}
	// then everything else
	else
	{
		return 1;
	}
}
