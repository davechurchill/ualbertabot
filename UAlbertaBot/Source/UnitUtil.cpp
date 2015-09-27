#include "UnitUtil.h"

using namespace UAlbertaBot;

bool UnitUtil::IsCombatUnit(BWAPI::Unit unit)
{
    UAB_ASSERT(unit != nullptr, "Unit was null");
    if (!unit)
    {
        return false;
    }

    // no workers or buildings allowed
    if (unit && unit->getType().isWorker() || unit->getType().isBuilding())
    {
        return false;
    }

    // check for various types of combat units
    if (unit->getType().canAttack() || 
        unit->getType() == BWAPI::UnitTypes::Terran_Medic ||
        unit->getType() == BWAPI::UnitTypes::Protoss_High_Templar ||
        unit->getType() == BWAPI::UnitTypes::Protoss_Observer)
    {
        return true;
    }
		
    return false;
}

bool UnitUtil::IsValidUnit(BWAPI::Unit unit)
{
    if (!unit)
    {
        return false;
    }

    if (unit->isCompleted() 
        && unit->getHitPoints() > 0 
        && unit->exists() 
        && unit->getType() != BWAPI::UnitTypes::Unknown 
        && unit->getPosition().x != BWAPI::Positions::Unknown.x 
        && unit->getPosition().y != BWAPI::Positions::Unknown.y) 
    {
        return true;
    }
    else
    {
        return false;
    }
}

Rect UnitUtil::GetRect(BWAPI::Unit unit)
{
    Rect r;

    r.x = unit->getLeft();
    r.y = unit->getTop();
    r.height = unit->getBottom() - unit->getTop();
    r.width = unit->getLeft() - unit->getRight();
}

double UnitUtil::GetDistanceBetweenTwoRectangles(Rect & rect1, Rect & rect2)
{
    Rect & mostLeft = rect1.x < rect2.x ? rect1 : rect2;
    Rect & mostRight = rect2.x < rect1.x ? rect1 : rect2;
    Rect & upper = rect1.y < rect2.y ? rect1 : rect2;
    Rect & lower = rect2.y < rect1.y ? rect1 : rect2;

    int diffX = std::max(0, mostLeft.x == mostRight.x ? 0 : mostRight.x - (mostLeft.x + mostLeft.width));
    int diffY = std::max(0, upper.y == lower.y ? 0 : lower.y - (upper.y + upper.height));
    
    return std::sqrtf(static_cast<float>(diffX*diffX + diffY*diffY));
}

bool UnitUtil::CanAttack(BWAPI::Unit attacker, BWAPI::Unit target)
{
    return GetWeapon(attacker, target) != BWAPI::UnitTypes::None;
}

bool UnitUtil::CanAttackAir(BWAPI::Unit unit)
{
    return unit->getType().airWeapon() != BWAPI::WeaponTypes::None;
}

bool UnitUtil::CanAttackGround(BWAPI::Unit unit)
{
    return unit->getType().groundWeapon() != BWAPI::WeaponTypes::None;
}

double UnitUtil::CalculateLTD(BWAPI::Unit attacker, BWAPI::Unit target)
{
    BWAPI::WeaponType weapon = GetWeapon(attacker, target);

    if (weapon == BWAPI::WeaponTypes::None)
    {
        return 0;
    }

    return static_cast<double>(weapon.damageAmount()) / weapon.damageCooldown();
}

BWAPI::WeaponType UnitUtil::GetWeapon(BWAPI::Unit attacker, BWAPI::Unit target)
{
    return target->isFlying() ? attacker->getType().airWeapon() : attacker->getType().groundWeapon();
}

BWAPI::WeaponType UnitUtil::GetWeapon(BWAPI::UnitType attacker, BWAPI::UnitType target)
{
    return target.isFlyer() ? attacker.airWeapon() : attacker.groundWeapon();
}

int UnitUtil::GetAttackRange(BWAPI::Unit attacker, BWAPI::Unit target)
{
    BWAPI::WeaponType weapon = GetWeapon(attacker, target);

    if (weapon == BWAPI::WeaponTypes::None)
    {
        return 0;
    }

    int range = weapon.maxRange();

    if ((attacker->getType() == BWAPI::UnitTypes::Protoss_Dragoon) 
        && (attacker->getPlayer() == BWAPI::Broodwar->self())
        && BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Singularity_Charge))
	{
		range = 6 * 32;
	}

    return range;
}

int UnitUtil::GetAttackRange(BWAPI::UnitType attacker, BWAPI::UnitType target)
{
    BWAPI::WeaponType weapon = GetWeapon(attacker, target);

    if (weapon == BWAPI::WeaponTypes::None)
    {
        return 0;
    }

    return weapon.maxRange();
}