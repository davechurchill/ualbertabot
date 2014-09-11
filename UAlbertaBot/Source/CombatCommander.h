#pragma once

#include "Common.h"
#include "Squad.h"
#include "SquadData.h"
#include "InformationManager.h"
#include "StrategyManager.h"

namespace UAlbertaBot
{
class CombatCommander
{
	SquadData			squadData;

	bool				attacking;
	bool				foundEnemy;
	bool				attackSent;
	
	int					selfTotalDeadUnits;
	int					numUnitsNeededForAttack;

	// Functions

    void				assignScoutDefenseSquads();
	void				assignDefenseSquads(std::set<BWAPI::Unit *> & combatUnits);
	void				assignAttackSquads(std::set<BWAPI::Unit *> & combatUnits);
	void				assignIdleSquads(std::set<BWAPI::Unit *> & combatUnits);

	void				assignAttackRegion(std::set<BWAPI::Unit *> & unitsToAssign);
	void				assignAttackVisibleUnits(std::set<BWAPI::Unit *> & unitsToAssign);
	void				assignAttackKnownBuildings(std::set<BWAPI::Unit *> & unitsToAssign);
	void				assignAttackExplore(std::set<BWAPI::Unit *> & unitsToAssign);

	bool				isBuildingAtBaselocation(BWTA::BaseLocation * baseLocation);
	bool				squadUpdateFrame();

	int					getNumType(UnitVector & units, BWAPI::UnitType type);

	BWAPI::Unit *		findClosestDefender(std::set<BWAPI::Unit *> & enemyUnitsInRegion, const std::set<BWAPI::Unit *> & units);
	BWTA::Region *		getClosestEnemyRegion();
	BWAPI::Position		getDefendLocation();

public:

	CombatCommander();

	// pass by value is not a typo, saves a line of copying into new set
	void update(std::set<BWAPI::Unit *> unitsToAssign);
	bool enemyInOurBase();

	void onRemoveUnit(BWAPI::Unit * unit);
	void onUnitShow(BWAPI::Unit * unit);
	void onUnitCreate(BWAPI::Unit * unit);
	void onUnitHide(BWAPI::Unit * unit);
	void onUnitMorph(BWAPI::Unit * unit);
	void onUnitRenegade(BWAPI::Unit * unit);
	void handleEvent(int eventType);

	void drawSquadInformation(int x, int y);
};
}