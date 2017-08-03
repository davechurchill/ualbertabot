#include "DetectorManager.h"
#include "Global.h"
#include "UnitUtil.h"
#include "Micro.h"
#include <boost/range/adaptors.hpp>

using namespace UAlbertaBot;

DetectorManager::DetectorManager(const AKBot::OpponentView& opponentView, const MapTools& mapTools, const BaseLocationManager& bases)
    : MicroManager(opponentView, bases)
	, unitClosestToEnemy(nullptr)
	, _mapTools(mapTools)
	, _bases(bases)
{ 
}

void DetectorManager::executeMicro(const std::vector<BWAPI::Unit> & targets, int currentFrame)
{
	const std::vector<BWAPI::Unit> & detectorUnits = getUnits();

	if (detectorUnits.empty())
	{
		return;
	}

	for (size_t i(0); i<targets.size(); ++i)
	{
		// do something here if there's targets
	}

	cloakedUnitMap.clear();
	std::vector<BWAPI::Unit> cloakedUnits;

	// figure out targets
	for (auto & unit : UnitUtil::getEnemyUnits())
	{
		// conditions for targeting
		if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker ||
			unit->getType() == BWAPI::UnitTypes::Protoss_Dark_Templar ||
			unit->getType() == BWAPI::UnitTypes::Terran_Wraith) 
		{
			cloakedUnits.push_back(unit);
			cloakedUnitMap[unit] = false;
		}
	}

	bool detectorUnitInBattle = false;

	// for each detectorUnit
	for (auto & detectorUnit : detectorUnits)
	{
		// if we need to regroup, move the detectorUnit to that location
		if (!detectorUnitInBattle && unitClosestToEnemy && unitClosestToEnemy->getPosition().isValid())
		{
			Micro::SmartMove(detectorUnit, unitClosestToEnemy->getPosition(), currentFrame);
			detectorUnitInBattle = true;
		}
		// otherwise there is no battle or no closest to enemy so we don't want our detectorUnit to die
		// send him to scout around the map
		else
		{
			auto explorePosition = _mapTools.getLeastRecentlySeenPosition(_bases);
			Micro::SmartMove(detectorUnit, explorePosition, currentFrame);
		}
	}
}


BWAPI::Unit DetectorManager::closestCloakedUnit(const std::vector<BWAPI::Unit> & cloakedUnits, BWAPI::Unit detectorUnit)
{
	using boost::adaptors::filtered;
	using boost::adaptors::transformed;
	auto cloakedUnitsNotInTheList = cloakedUnits
		| filtered([this](const BWAPI::Unit& unit) { 
			return !cloakedUnitMap[unit];
		});
	/*
	 This distance to observer map 
	auto distanceToObserverMap = cloakedUnitsNotInTheList | transformed([&detectorUnit](const BWAPI::Unit const& unit) {
		return std::make_pair(unit, unit->getDistance(detectorUnit));
	});
	*/
	return UnitUtil::GetClosestsUnitToTarget(cloakedUnitsNotInTheList, detectorUnit);
}