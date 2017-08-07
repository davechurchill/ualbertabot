#include "Micro.h"
#include "UnitUtil.h"
#include <BWAPI/UnitCommand.h>
#include <BWAPI/Game.h>
#include <BWAPI/Player.h>
#include <BWAPI/TechType.h>

using namespace UAlbertaBot;

size_t TotalCommands = 0;

const int dotRadius = 2;

std::function<void(const BWAPI::Unit& attacker, const BWAPI::Unit& target)> onAttackHandler;
std::function<void(const BWAPI::Unit& attacker, const BWAPI::Position & targetPosition)> onAttackMoveHandler;
std::function<void(const BWAPI::Unit& attacker, const BWAPI::Position & targetPosition)> onMoveHandler;
std::function<void(const BWAPI::Unit& unit, const BWAPI::Unit& target)> onRepairHandler;
std::function<void(const BWAPI::Unit& unit, const BWAPI::Unit& target)> onRightClickHandler;

void Micro::drawAPM(AKBot::ScreenCanvas& canvas, int x, int y)
{
    int bwapiAPM = BWAPI::Broodwar->getAPM();
    int myAPM = (int)(TotalCommands / ((double)BWAPI::Broodwar->getFrameCount() / (24*60)));
    canvas.drawTextScreen(x, y, "%d %d", bwapiAPM, myAPM);
}

void Micro::drawAction(
	AKBot::ScreenCanvas& canvas,
	const BWAPI::Position& attackerPostion,
	const BWAPI::Position & targetPosition,
	const BWAPI::Color color)
{
	if (Config.Debug.DrawUnitTargetInfo)
	{
		canvas.drawCircleMap(attackerPostion, dotRadius, color, true);
		canvas.drawCircleMap(targetPosition, dotRadius, color, true);
		canvas.drawLineMap(attackerPostion, targetPosition, color);
	}
}

void UAlbertaBot::Micro::SetOnAttackUnit(std::function<void(const BWAPI::Unit&attacker, const BWAPI::Unit&target)> handler)
{
	onAttackHandler = handler;
}

void UAlbertaBot::Micro::SetOnAttackMove(std::function<void(const BWAPI::Unit&attacker, const BWAPI::Position&targetPosition)> handler)
{
	onAttackMoveHandler = handler;
}

void UAlbertaBot::Micro::SetOnMove(std::function<void(const BWAPI::Unit&attacker, const BWAPI::Position&targetPosition)> handler)
{
	onMoveHandler = handler;
}

void UAlbertaBot::Micro::SetOnRepair(std::function<void(const BWAPI::Unit&unit, const BWAPI::Unit&target)> handler)
{
	onRepairHandler = handler;
}

void UAlbertaBot::Micro::SetOnRightClick(std::function<void(const BWAPI::Unit&unit, const BWAPI::Unit&target)> handler)
{
	onRightClickHandler = handler;
}

void Micro::SmartAttackUnit(BWAPI::Unit attacker, BWAPI::Unit target, int currentFrame)
{
    UAB_ASSERT(attacker, "SmartAttackUnit: Attacker not valid");
    UAB_ASSERT(target, "SmartAttackUnit: Target not valid");

    if (!attacker || !target)
    {
        return;
    }

    // if we have issued a command to this unit already this frame, ignore this one
    if (attacker->getLastCommandFrame() >= currentFrame || attacker->isAttackFrame())
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
    TotalCommands++;

	if (onAttackHandler)
	{
		onAttackHandler(attacker, target);
	}
}

void Micro::SmartAttackMove(BWAPI::Unit attacker, const BWAPI::Position & targetPosition, int currentFrame)
{
    //UAB_ASSERT(attacker, "SmartAttackMove: Attacker not valid");
    //UAB_ASSERT(targetPosition.isValid(), "SmartAttackMove: targetPosition not valid");

    if (!attacker || !targetPosition.isValid())
    {
        return;
    }

    // if we have issued a command to this unit already this frame, ignore this one
    if (attacker->getLastCommandFrame() >= currentFrame || attacker->isAttackFrame())
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
    TotalCommands++;

	if (onAttackMoveHandler)
	{
		onAttackMoveHandler(attacker, targetPosition);
	}
}

void Micro::SmartMove(BWAPI::Unit attacker, const BWAPI::Position & targetPosition, int currentFrame)
{
    //UAB_ASSERT(attacker, "SmartAttackMove: Attacker not valid");
    //UAB_ASSERT(targetPosition.isValid(), "SmartAttackMove: targetPosition not valid");

    if (!attacker)
    {
        return;
    }

    // if we have issued a command to this unit already this frame, ignore this one
    if (attacker->getLastCommandFrame() >= currentFrame)
    {
        return;
    }

    // get the unit's current command
    BWAPI::UnitCommand currentCommand(attacker->getLastCommand());

    // if we've already told this unit to move to this position, ignore this command
    if ((currentCommand.getType() == BWAPI::UnitCommandTypes::Move) && (currentCommand.getTargetPosition() == targetPosition) && attacker->isMoving())
    {
        return;
    }

    // if nothing prevents it, attack the target
    attacker->move(targetPosition);
    TotalCommands++;

	if (onMoveHandler)
	{
		onMoveHandler(attacker, targetPosition);
	}
}

void Micro::SmartRightClick(BWAPI::Unit unit, BWAPI::Unit target, int currentFrame)
{
    UAB_ASSERT(unit, "SmartRightClick: Unit not valid");
    UAB_ASSERT(target, "SmartRightClick: Target not valid");

    if (!unit || !target)
    {
        return;
    }

    // if we have issued a command to this unit already this frame, ignore this one
    if (unit->getLastCommandFrame() >= currentFrame || unit->isAttackFrame())
    {
        return;
    }

    // get the unit's current command
    BWAPI::UnitCommand currentCommand(unit->getLastCommand());

    // if we've already told this unit to move to this position, ignore this command
    if ((currentCommand.getType() == BWAPI::UnitCommandTypes::Right_Click_Unit) && (currentCommand.getTargetPosition() == target->getPosition()))
    {
        return;
    }

    // if nothing prevents it, attack the target
    unit->rightClick(target);
    TotalCommands++;

	if (onRightClickHandler)
	{
		onRightClickHandler(unit, target);
	}
}

void Micro::SmartLaySpiderMine(BWAPI::Unit unit, BWAPI::Position pos)
{
    if (!unit)
    {
        return;
    }

    if (!unit->canUseTech(BWAPI::TechTypes::Spider_Mines, pos))
    {
        return;
    }

    BWAPI::UnitCommand currentCommand(unit->getLastCommand());

    // if we've already told this unit to move to this position, ignore this command
    if ((currentCommand.getType() == BWAPI::UnitCommandTypes::Use_Tech_Position) && (currentCommand.getTargetPosition() == pos))
    {
        return;
    }

    unit->canUseTechPosition(BWAPI::TechTypes::Spider_Mines, pos);
}

void Micro::SmartRepair(BWAPI::Unit unit, BWAPI::Unit target, int currentFrame)
{
    UAB_ASSERT(unit, "SmartRightClick: Unit not valid");
    UAB_ASSERT(target, "SmartRightClick: Target not valid");

    if (!unit || !target)
    {
        return;
    }

    // if we have issued a command to this unit already this frame, ignore this one
    if (unit->getLastCommandFrame() >= currentFrame || unit->isAttackFrame())
    {
        return;
    }

    // get the unit's current command
    BWAPI::UnitCommand currentCommand(unit->getLastCommand());

    // if we've already told this unit to move to this position, ignore this command
    if ((currentCommand.getType() == BWAPI::UnitCommandTypes::Repair) && (currentCommand.getTarget() == target))
    {
        return;
    }

    // if nothing prevents it, attack the target
    unit->repair(target);
    TotalCommands++;

	if (onRepairHandler)
	{
		onRepairHandler(unit, target);
	}
}

void Micro::SmartKiteTarget(BWAPI::Unit rangedUnit, BWAPI::Unit target, int currentFrame)
{
    UAB_ASSERT(rangedUnit, "SmartKiteTarget: Unit not valid");
    UAB_ASSERT(target, "SmartKiteTarget: Target not valid");

    if (!rangedUnit || !target)
    {
        return;
    }

	double range(rangedUnit->getType().groundWeapon().maxRange());
	if (rangedUnit->getType() == BWAPI::UnitTypes::Protoss_Dragoon && BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Singularity_Charge))
	{
		range = 6*32;
	}

	// determine whether the target can be kited
    bool kiteLonger = Config.Micro.KiteLongerRangedUnits.find(rangedUnit->getType()) != Config.Micro.KiteLongerRangedUnits.end();
	if (!kiteLonger && (range <= target->getType().groundWeapon().maxRange()))
	{
		// if we can't kite it, there's no point
		Micro::SmartAttackUnit(rangedUnit, target, currentFrame);
		return;
	}

	bool    kite(true);
	double  dist(rangedUnit->getDistance(target));
	double  speed(rangedUnit->getType().topSpeed());

    
    // if the unit can't attack back don't kite
    if ((rangedUnit->isFlying() && !UnitUtil::CanAttackAir(target)) || (!rangedUnit->isFlying() && !UnitUtil::CanAttackGround(target)))
    {
        //kite = false;
    }

	double timeToEnter = std::max(0.0,(dist - range) / speed);
	if ((timeToEnter >= rangedUnit->getGroundWeaponCooldown()))
	{
		kite = false;
	}

	if (target->getType().isBuilding())
	{
		kite = false;
	}

	// if we can't shoot, run away
	if (kite)
	{
		BWAPI::Position fleePosition(rangedUnit->getPosition() - target->getPosition() + rangedUnit->getPosition());
		//BWAPI::Broodwar->drawLineMap(rangedUnit->getPosition(), fleePosition, BWAPI::Colors::Cyan);
		Micro::SmartMove(rangedUnit, fleePosition, currentFrame);
	}
	// otherwise shoot
	else
	{
		Micro::SmartAttackUnit(rangedUnit, target, currentFrame);
	}
}

void Micro::MutaDanceTarget(BWAPI::Unit muta, BWAPI::Unit target)
{
    UAB_ASSERT(muta, "MutaDanceTarget: Muta not valid");
    UAB_ASSERT(target, "MutaDanceTarget: Target not valid");

    if (!muta || !target)
    {
        return;
    }

    const int cooldown                  = muta->getType().groundWeapon().damageCooldown();
    const int latency                   = BWAPI::Broodwar->getLatency();
    const double speed                  = muta->getType().topSpeed();
    const double range                  = muta->getType().groundWeapon().maxRange();
    const double distanceToTarget       = muta->getDistance(target);
	const double distanceToFiringRange  = std::max(distanceToTarget - range,0.0);
	const double timeToEnterFiringRange = distanceToFiringRange / speed;
	const int framesToAttack            = static_cast<int>(timeToEnterFiringRange) + 2*latency;

	// How many frames are left before we can attack?
	const int currentCooldown = muta->isStartingAttack() ? cooldown : muta->getGroundWeaponCooldown();

	BWAPI::Position fleeVector = GetKiteVector(target, muta);
	BWAPI::Position moveToPosition(muta->getPosition() + fleeVector);

	// If we can attack by the time we reach our firing range
	if(currentCooldown <= framesToAttack)
	{
		// Move towards and attack the target
		muta->attack(target);
	}
	else // Otherwise we cannot attack and should temporarily back off
	{
		// Determine direction to flee
		// Determine point to flee to
		if (moveToPosition.isValid()) 
		{
			muta->rightClick(moveToPosition);
		}
	}
}

BWAPI::Position Micro::GetKiteVector(BWAPI::Unit unit, BWAPI::Unit target)
{
    BWAPI::Position fleeVec(target->getPosition() - unit->getPosition());
    double fleeAngle = atan2(fleeVec.y, fleeVec.x);
    fleeVec = BWAPI::Position(static_cast<int>(64 * cos(fleeAngle)), static_cast<int>(64 * sin(fleeAngle)));
    return fleeVec;
}

const double PI = 3.14159265;
void Micro::Rotate(double &x, double &y, double angle)
{
	angle = angle*PI/180.0;
	x = (x * cos(angle)) - (y * sin(angle));
	y = (y * cos(angle)) + (x * sin(angle));
}

void Micro::Normalize(double &x, double &y)
{
	double length = sqrt((x * x) + (y * y));
	if (length != 0)
	{
		x = (x / length);
		y = (y / length);
	}
}