#include "UnitUtil.h"

using namespace UAlbertaBot;

bool UnitUtil::IsCombatUnit(BWAPI::UnitInterface * unit)
{
    UAB_ASSERT(unit != NULL, "Unit was null");
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

bool UnitUtil::IsValidUnit(BWAPI::UnitInterface * unit)
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

bool UnitUtil::CanAttackAir(BWAPI::UnitInterface * unit)
{
    return unit->getType().airWeapon() != BWAPI::WeaponTypes::None;
}

bool UnitUtil::CanAttackGround(BWAPI::UnitInterface * unit)
{
    return unit->getType().groundWeapon() != BWAPI::WeaponTypes::None;
}
