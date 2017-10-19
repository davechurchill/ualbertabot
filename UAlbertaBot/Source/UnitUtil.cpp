#include "UnitUtil.h"
#include "UABAssert.h"
#include <BWAPI/Game.h>
#include <BWAPI/Player.h>
#include <BWAPI/Playerset.h>
#include <BWAPI/Unitset.h>
#include <BWAPI/UnitCommand.h>

using namespace UAlbertaBot;

bool UnitUtil::IsCombatUnit(BWAPI::Unit unit)
{
    UAB_ASSERT(unit != nullptr, "Unit was null");
    if (!unit)
    {
        return false;
    }

    return IsCombatUnitType(unit->getType());

}

bool UnitUtil::IsCombatUnitType(BWAPI::UnitType type)
{
    // check for various types of combat units
    if (type.canAttack() || 
        type == BWAPI::UnitTypes::Terran_Medic ||
        type == BWAPI::UnitTypes::Protoss_High_Templar ||
        type == BWAPI::UnitTypes::Protoss_Observer ||
        type == BWAPI::UnitTypes::Zerg_Overlord ||
        type == BWAPI::UnitTypes::Protoss_Observer)
    {
        return true;
    }

    // no workers or buildings allowed
    if (type.isWorker() || type.isBuilding())
    {
        return false;
    }

    return false;
}

BWAPI::Position UnitUtil::GetUnitsetCenter(const std::vector<BWAPI::Unit> & cluster)
{
    int sumX = 0;
    int sumY = 0;

    for (const auto & resource : cluster)
    {
        sumX += resource->getPosition().x;
        sumY += resource->getPosition().y;
    }

    return BWAPI::Position(sumX / cluster.size(), sumY / cluster.size());
}

bool UnitUtil::IsMorphedBuildingType(BWAPI::UnitType type)
{
    return  type == BWAPI::UnitTypes::Zerg_Sunken_Colony ||
            type == BWAPI::UnitTypes::Zerg_Spore_Colony ||
            type == BWAPI::UnitTypes::Zerg_Lair ||
            type == BWAPI::UnitTypes::Zerg_Hive ||
            type == BWAPI::UnitTypes::Zerg_Greater_Spire;
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

    return r;
}

UnitCollection UAlbertaBot::UnitUtil::getEnemyUnits(shared_ptr<AKBot::OpponentView> opponentView)
{
#if _MSC_VER < 1900
	UnitCollection result;
#endif
	for (const auto& enemy : opponentView->enemies())
	{
		for (const auto & unit : enemy->getUnits())
		{
#if _MSC_VER < 1900
			result.push_back(unit);
#else
			co_yield unit;
#endif
		}
	}

#if _MSC_VER < 1900
	return result;
#endif
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
    return GetWeapon(attacker, target) != BWAPI::WeaponTypes::None;
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
	auto self = BWAPI::Broodwar->self();
    if ((attacker->getType() == BWAPI::UnitTypes::Protoss_Dragoon) 
        && (attacker->getPlayer() == self)
        && self->getUpgradeLevel(BWAPI::UpgradeTypes::Singularity_Charge))
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

size_t UnitUtil::GetAllUnitCount(BWAPI::UnitType type)
{
    size_t count = 0;
    for (const auto & unit : BWAPI::Broodwar->self()->getUnits())
    {
        // trivial case: unit which exists matches the type
        if (unit->getType() == type)
        {
            count++;
        }

        // case where a zerg egg contains the unit type
        if (unit->getType() == BWAPI::UnitTypes::Zerg_Egg && unit->getBuildType() == type)
        {
            count += type.isTwoUnitsInOneEgg() ? 2 : 1;
        }

        // case where a building has started constructing a unit but it doesn't yet have a unit associated with it
        if (unit->getRemainingTrainTime() > 0)
        {
            BWAPI::UnitType trainType = unit->getLastCommand().getUnitType();

            if (trainType == type && unit->getRemainingTrainTime() == trainType.buildTime())
            {
                count++;
            }
        }
    }

    return count;
}

const BWAPI::UnitType UnitUtil::getResourceDepot(const BWAPI::Race& race)
{
	using namespace BWAPI::UnitTypes::Enum;
	static const int baseTypes[BWAPI::Races::Enum::MAX] =
	{
		Zerg_Hatchery, Terran_Command_Center, Protoss_Nexus,
		None, None, None, // unused
		Unknown, None, Unknown // random, none, unk
	};
	return baseTypes[race.getID()];
}

void UAlbertaBot::UnitUtil::getUnitsInRadius(
	shared_ptr<AKBot::OpponentView> opponentView,
	std::vector<BWAPI::Unit>& units,
	BWAPI::Position center,
	int radius,
	bool ourUnits,
	bool oppUnits)
{
	if (ourUnits)
	{
		for (const BWAPI::Unit & unit : BWAPI::Broodwar->self()->getUnits())
		{
			if (unit->getPosition().getDistance(center) <= radius)
			{
				units.push_back(unit);
			}
		}
	}

	if (oppUnits)
	{
		for (const auto & unit : UnitUtil::getEnemyUnits(opponentView))
		{
			if (unit->getPosition().getDistance(center) <= radius)
			{
				units.push_back(unit);
			}
		}
	}
}
