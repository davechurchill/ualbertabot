#include "DetectorManager.h"

using namespace UAlbertaBot;

DetectorManager::DetectorManager() : unitClosestToEnemy(NULL) { }

void DetectorManager::executeMicro(const std::vector<BWAPI::UnitInterface *> & targets) 
{
	const std::vector<BWAPI::UnitInterface *> & detectorUnits = getUnits();
	/*
	TODO: check if each unit's radius contains enemy/biological/energy>0/shield>0/notirradiated depending on race
		calculate best value through evaluation function of number of units and perhaps hp/stats
		cast emp/irradiate on best enemy unit
	otherwise matrix most hp attacking unit mech > bio

	eraser: cast irradiate on each mech unit and move around #unfeasible?
	*/
	if (detectorUnits.empty())
	{
		return;
	}
	BWAPI::UnitInterface* detector = detectorUnits[0];
	if (detector->canUseTech(BWAPI::TechTypes::Defensive_Matrix, NULL)) //Check if it's a Science Vessel
	{
		BWAPI::Race enemyRace = (BWAPI::Broodwar->enemy()->getRace());
		if (enemyRace == BWAPI::Races::Zerg)
		{
			std::vector<BWAPI::UnitInterface *> spellUnitTargets;
			for (size_t i(0); i<targets.size(); ++i)
			{
					// do something here if there's targets
				BWAPI::Unitset myEnemies = targets[i]->getUnitsInRadius(64,!BWAPI::Filter::IsIrradiated
					&& BWAPI::Filter::IsOrganic
					&& BWAPI::Filter::IsVisible);
					if (!myEnemies.empty()) {
						if (myEnemies.size() > 3) {
							if (detector->canUseTech(BWAPI::TechTypes::Irradiate, NULL)) detector->useTech(BWAPI::TechTypes::Irradiate, myEnemies.getClosestUnit(nullptr,128));
							else {
								BWAPI::Unit ally = BWAPI::Broodwar->getClosestUnit(detector->getTargetPosition(), BWAPI::Filter::IsOwned);
								detector->useTech(BWAPI::TechTypes::Defensive_Matrix, ally);	BWAPI::Broodwar->printf("DETECTOR cast");

							}
							break;
						}
					}
				}
		}
		else if (enemyRace == BWAPI::Races::Terran)
		{
			std::vector<BWAPI::UnitInterface *> spellUnitTargets;
			for (size_t i(0); i<targets.size(); ++i)
			{
				// do something here if there's targets
				BWAPI::Unitset myEnemies = targets[i]->getUnitsInRadius(64, !BWAPI::Filter::IsIrradiated
					&& BWAPI::Filter::IsOrganic
					&& BWAPI::Filter::IsVisible);
				if (!myEnemies.empty()) {
					if (myEnemies.size() > 3) { 
						if (detector->canUseTech(BWAPI::TechTypes::Irradiate, NULL)) detector->useTech(BWAPI::TechTypes::Irradiate, myEnemies.getClosestUnit(nullptr, 128));
						else {
							BWAPI::Unit ally = BWAPI::Broodwar->getClosestUnit(detector->getTargetPosition(), BWAPI::Filter::IsOwned );
							detector->useTech(BWAPI::TechTypes::Defensive_Matrix, ally);
						}
						break;
					}
				}
			}
		}
		else if (enemyRace == BWAPI::Races::Protoss)
		{
			std::vector<BWAPI::UnitInterface *> spellUnitTargets;
			for (size_t i(0); i<targets.size(); ++i)
			{
				// do something here if there's targets
				BWAPI::Unitset myEnemies = targets[i]->getUnitsInRadius(96, BWAPI::Filter::Shields);
				if (!myEnemies.empty()) {
					if (myEnemies.size() > 3) {
						if (detector->canUseTech(BWAPI::TechTypes::Irradiate, NULL)) detector->useTech(BWAPI::TechTypes::Irradiate, myEnemies.getClosestUnit(nullptr, 128));
						else {
							BWAPI::Unit ally = BWAPI::Broodwar->getClosestUnit(detector->getTargetPosition(), BWAPI::Filter::IsOwned );
							detector->useTech(BWAPI::TechTypes::Defensive_Matrix, ally);
						}
						break;
					}
				}
			}
		}
	}

	cloakedUnitMap.clear();
	std::vector<BWAPI::UnitInterface *> cloakedUnits;

	// figure out targets
	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->enemy()->getUnits())
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
	for (BWAPI::UnitInterface* detectorUnit : detectorUnits)
	{
		// if we need to regroup, move the detectorUnit to that location
		if (!detectorUnitInBattle && unitClosestToEnemy && unitClosestToEnemy->getPosition().isValid())
		{
			smartMove(detectorUnit, unitClosestToEnemy->getPosition());
			detectorUnitInBattle = true;
		}
		// otherwise there is no battle or no closest to enemy so we don't want our detectorUnit to die
		// send him to scout around the map
		else
		{
			BWAPI::Position explorePosition = MapGrid::Instance().getLeastExplored();
			smartMove(detectorUnit, explorePosition);
		}
	}
}


BWAPI::UnitInterface* DetectorManager::closestCloakedUnit(const std::vector<BWAPI::UnitInterface *> & cloakedUnits, BWAPI::UnitInterface* detectorUnit)
{
	BWAPI::UnitInterface* closestCloaked = NULL;
	double closestDist = 100000;

	for (BWAPI::UnitInterface* unit : cloakedUnits)
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
