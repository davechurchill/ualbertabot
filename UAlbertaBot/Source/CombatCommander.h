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
	void				assignDefenseSquads(std::set<BWAPI::UnitInterface*> & combatUnits);
	void				assignAttackSquads(std::set<BWAPI::UnitInterface*> & combatUnits);
	void				assignIdleSquads(std::set<BWAPI::UnitInterface*> & combatUnits);

	void				assignAttackRegion(std::set<BWAPI::UnitInterface*> & unitsToAssign);
	void				assignAttackVisibleUnits(std::set<BWAPI::UnitInterface*> & unitsToAssign);
	void				assignAttackKnownBuildings(std::set<BWAPI::UnitInterface*> & unitsToAssign);
	void				assignAttackExplore(std::set<BWAPI::UnitInterface*> & unitsToAssign);

	bool				isBuildingAtBaselocation(BWTA::BaseLocation * baseLocation);
	bool				squadUpdateFrame();

	int					getNumType(std::vector<BWAPI::UnitInterface *> & units, BWAPI::UnitType type);

	BWAPI::UnitInterface*		findClosestDefender(std::set<BWAPI::UnitInterface*> & enemyUnitsInRegion, const std::set<BWAPI::UnitInterface*> & units);
	BWTA::Region *		getClosestEnemyRegion();
	BWAPI::Position		getDefendLocation();

public:

	CombatCommander();

	// pass by value is not a typo, saves a line of copying into new set
	void update(std::set<BWAPI::UnitInterface*> unitsToAssign);
	bool enemyInOurBase();

	void onRemoveUnit(BWAPI::UnitInterface* unit);
	void onUnitShow(BWAPI::UnitInterface* unit);
	void onUnitCreate(BWAPI::UnitInterface* unit);
	void onUnitHide(BWAPI::UnitInterface* unit);
	void onUnitMorph(BWAPI::UnitInterface* unit);
	void onUnitRenegade(BWAPI::UnitInterface* unit);
	void handleEvent(int eventType);

	void drawSquadInformation(int x, int y);
};
}