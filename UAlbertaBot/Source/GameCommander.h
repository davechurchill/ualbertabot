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
	CombatCommander		    _combatCommander;

	ScoutManager		    _scoutManager;
	TimerManager		    _timerManager;
    
	BWAPI::Unitset          _validUnits;
	BWAPI::Unitset          _combatUnits;
	BWAPI::Unitset          _scoutUnits;

    bool                    _initialScoutSet;

    void                    assignUnit(BWAPI::UnitInterface * unit, BWAPI::Unitset & set);
	bool                    isAssigned(BWAPI::UnitInterface* unit) const;

public:

	GameCommander();
	~GameCommander() {};

	void update();

	void handleUnitAssignments();
	void setValidUnits();
	void setScoutUnits();
	void setCombatUnits();

	void drawDebugInterface();

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