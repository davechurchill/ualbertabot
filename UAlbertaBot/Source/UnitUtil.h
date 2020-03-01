#pragma once

#include <BWAPI.h>

namespace UAlbertaBot
{

struct Rect
{
    int x, y;
    int height, width;
};

namespace UnitUtil
{      
    bool IsCombatUnit(BWAPI::Unit unit);
    bool IsValidUnit(BWAPI::Unit unit);
    bool CanAttackAir(BWAPI::Unit unit);
    bool CanAttackGround(BWAPI::Unit unit);
    bool CanAttack(BWAPI::Unit attacker, BWAPI::Unit target);
    int GetAttackRange(BWAPI::Unit attacker, BWAPI::Unit target);
    int GetAttackRange(BWAPI::UnitType attacker, BWAPI::UnitType target);
    double CalculateLTD(BWAPI::Unit attacker, BWAPI::Unit target);

    size_t GetAllUnitCount(BWAPI::UnitType type);
        
    BWAPI::Unit GetClosestUnitTypeToTarget(BWAPI::UnitType type, BWAPI::Position target);
    BWAPI::WeaponType GetWeapon(BWAPI::Unit attacker, BWAPI::Unit target);
    BWAPI::WeaponType GetWeapon(BWAPI::UnitType attacker, BWAPI::UnitType target);

    double GetDistanceBetweenTwoRectangles(Rect & rect1, Rect & rect2);
    Rect GetRect(BWAPI::Unit unit);
};
}