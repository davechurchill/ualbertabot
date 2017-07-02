#include "DetectorManager.h"
#include "Global.h"
#include "UnitUtil.h"

using namespace UAlbertaBot;

DetectorManager::DetectorManager() 
    : unitClosestToEnemy(nullptr) 
{ 
}

void DetectorManager::executeMicro(const std::vector<BWAPI::Unit> & targets) 
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
			Micro::SmartMove(detectorUnit, unitClosestToEnemy->getPosition());
			detectorUnitInBattle = true;
		}
		// otherwise there is no battle or no closest to enemy so we don't want our detectorUnit to die
		// send him to scout around the map
		else
		{
			auto explorePosition = Global::Map().getLeastRecentlySeenPosition(Global::Bases());
			Micro::SmartMove(detectorUnit, explorePosition);
		}
	}
}

BWAPI::Unit DetectorManager::closestCloakedUnit(const std::vector<BWAPI::Unit> & cloakedUnits, BWAPI::Unit detectorUnit)
{
	BWAPI::Unit closestCloaked = nullptr;
	double closestDist = 100000;

	for (auto & unit : cloakedUnits)
	{
		// if we haven't already assigned an detectorUnit to this cloaked unit
		if (!cloakedUnitMap[unit])
		{
			double dist = unit->getDistance(detectorUnit);

			if (!closestCloaked || (dist < closestDist))
			{
				closestCloaked = unit;
				closestDist = dist;
			}
		}
	}

	return closestCloaked;
}