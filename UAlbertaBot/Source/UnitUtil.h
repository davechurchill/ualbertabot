#pragma once

#include <BWAPI/Unit.h>
#if _MSC_VER >= 1900
#include <experimental/generator>
typedef std::experimental::generator<BWAPI::Unit> UnitCollection;
#else
#include <vector>
typedef std::vector<BWAPI::Unit> UnitCollection;
#endif
#include <numeric>
#include <memory>
#include "OpponentView.h"

using std::shared_ptr;

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
    int GetAttackRange(
		BWAPI::Player self,
		BWAPI::Unit attacker,
		BWAPI::Unit target);
    int GetAttackRange(BWAPI::UnitType attacker, BWAPI::UnitType target);
    
    size_t GetAllUnitCount(
		BWAPI::Player self,
		BWAPI::UnitType type);
    BWAPI::Position GetUnitsetCenter(const std::vector<BWAPI::Unit> & cluster);
    
    // BWAPI::Unit GetClosestUnitTypeToTarget(BWAPI::UnitType type, BWAPI::Position target);
	template<typename TUnitCollection>
	BWAPI::Unit GetClosestsUnit(
		const TUnitCollection& units,
		std::function<bool(const BWAPI::Unit&)> distance)
	{
		double closestDist = std::numeric_limits<double>::infinity();
		BWAPI::Unit closestUnit = nullptr;
		return std::accumulate(units.begin(), units.end(), closestUnit, [&closestDist, &distance](BWAPI::Unit init, BWAPI::Unit unit) {
			double dist = distance(unit);
			if (init == nullptr) {
				closestDist = dist;
				return unit;
			}

			if (closestDist > dist) {
				closestDist = dist;
				return unit;
			}

			return init;
		});
	};
	template<typename TUnitCollection, typename TTargetType>
	BWAPI::Unit GetClosestsUnitToTarget(
		const TUnitCollection& units,
		TTargetType target)
	{
		return GetClosestsUnit(units, [&target](const BWAPI::Unit& unit) {
			return unit->getDistance(target);
		});
	};
    BWAPI::WeaponType GetWeapon(BWAPI::Unit attacker, BWAPI::Unit target);
    BWAPI::WeaponType GetWeapon(BWAPI::UnitType attacker, BWAPI::UnitType target);

    double GetDistanceBetweenTwoRectangles(Rect & rect1, Rect & rect2);
    Rect GetRect(BWAPI::Unit unit);

	UnitCollection getEnemyUnits(shared_ptr<AKBot::OpponentView> opponentView);
	const BWAPI::UnitType getResourceDepot(const BWAPI::Race& race);

	// get units within radius of center and add to units
	void getUnitsInRadius(
		shared_ptr<AKBot::OpponentView> opponentView,
		std::vector<BWAPI::Unit> & units,
		BWAPI::Position center,
		int radius,
		bool ourUnits,
		bool oppUnits);
};
}