#pragma once

#include <BWAPI.h>
#if _MSC_VER >= 1900
#include <experimental/generator>
typedef std::experimental::generator<BWAPI::Unit> UnitCollection;
#else
#include <vector>
typedef std::vector<BWAPI::Unit> UnitCollection;
#endif

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
    bool IsCombatUnitType(BWAPI::UnitType unit);
    bool IsValidUnit(BWAPI::Unit unit);
    bool CanAttackAir(BWAPI::Unit unit);
    bool CanAttackGround(BWAPI::Unit unit);
    bool CanAttack(BWAPI::Unit attacker, BWAPI::Unit target);
    bool IsMorphedBuildingType(BWAPI::UnitType type);
    double CalculateLTD(BWAPI::Unit attacker, BWAPI::Unit target);
    int GetAttackRange(BWAPI::Unit attacker, BWAPI::Unit target);
    int GetAttackRange(BWAPI::UnitType attacker, BWAPI::UnitType target);
    
    size_t GetAllUnitCount(BWAPI::UnitType type);
    BWAPI::Position GetUnitsetCenter(const std::vector<BWAPI::Unit> & cluster);
    
    BWAPI::Unit GetClosestUnitTypeToTarget(BWAPI::UnitType type, BWAPI::Position target);
    BWAPI::WeaponType GetWeapon(BWAPI::Unit attacker, BWAPI::Unit target);
    BWAPI::WeaponType GetWeapon(BWAPI::UnitType attacker, BWAPI::UnitType target);

    double GetDistanceBetweenTwoRectangles(Rect & rect1, Rect & rect2);
    Rect GetRect(BWAPI::Unit unit);

	UnitCollection getEnemyUnits();
	const BWAPI::UnitType getResourceDepot(const BWAPI::Race& race);
};
}