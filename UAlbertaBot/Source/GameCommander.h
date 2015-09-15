#pragma once

#include "Common.h"
#include "CombatCommander.h"
#include "InformationManager.h"
#include "MapGrid.h"
#include "WorkerManager.h"
#include "ProductionManager.h"
#include "BuildingManager.h"
#include "ScoutManager.h"
#include "StrategyManager.h"
#include "TimerManager.h"

namespace UAlbertaBot
{


class UnitToAssign
{
public:

	BWAPI::UnitInterface* unit;
	bool isAssigned;

	UnitToAssign(BWAPI::UnitInterface* u)
	{
		unit = u;
		isAssigned = false;
	}
};

class GameCommander 
{
	CombatCommander		combatCommander;

	ScoutManager		scoutManager;
	TimerManager		timerManager;

	std::set<BWAPI::UnitInterface*> combatUnits;
	std::set<BWAPI::UnitInterface*> scoutUnits;
	std::set<BWAPI::UnitInterface*> workerUnits;

	std::set<BWAPI::UnitInterface*>	validUnits;
	std::set<BWAPI::UnitInterface*> assignedUnits;

	BWAPI::UnitInterface* currentScout;
	int numWorkerScouts;

	const bool isAssigned(BWAPI::UnitInterface* unit) const;

public:

	GameCommander();
	~GameCommander() {};

	void update();

	void populateUnitVectors();
	void setValidUnits();
	void setScoutUnits();
	void setWorkerUnits();
	void setCombatUnits();

	void drawDebugInterface();

	bool isValidUnit(BWAPI::UnitInterface* unit);
	bool isCombatUnit(BWAPI::UnitInterface* unit) const;

	BWAPI::UnitInterface* getFirstSupplyProvider();
	BWAPI::UnitInterface* getClosestUnitToTarget(BWAPI::UnitType type, BWAPI::Position target);
	BWAPI::UnitInterface* getClosestWorkerToTarget(BWAPI::Position target);

	void onUnitShow(BWAPI::UnitInterface* unit);
	void onUnitHide(BWAPI::UnitInterface* unit);
	void onUnitCreate(BWAPI::UnitInterface* unit);
	void onUnitComplete(BWAPI::UnitInterface* unit);
	void onUnitRenegade(BWAPI::UnitInterface* unit);
	void onUnitDestroy(BWAPI::UnitInterface* unit);
	void onUnitMorph(BWAPI::UnitInterface* unit);
};

}