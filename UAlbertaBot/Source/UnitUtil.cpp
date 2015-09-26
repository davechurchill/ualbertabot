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

bool UnitUtil::CanAttackAir(BWAPI::Unit unit)
{
    return unit->getType().airWeapon() != BWAPI::WeaponTypes::None;
}

bool UnitUtil::CanAttackGround(BWAPI::Unit unit)
{
    return unit->getType().groundWeapon() != BWAPI::WeaponTypes::None;
}

BWAPI::Unit GetTrainee(BWAPI::Unit trainer)
{
    if (trainer->getRemainingTrainTime() == 0)    
    {
        return nullptr;
    }

    for (const auto & u : trainer->getPlayer()->getUnits())
    {
        if (u->getRemainingBuildTime() == 0)
        {
            continue;
        }

        const BWAPI::Position & uPos = u->getPosition();

        if ((uPos.x > trainer->getLeft()) && (uPos.x < trainer->getRight()) && (uPos.y < trainer->getBottom()) && (uPos.y > trainer->getTop()))
        {
            return u;
        }
    }

    return nullptr;
}