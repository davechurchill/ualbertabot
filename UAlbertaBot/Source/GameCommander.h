#pragma once

#include "Common.h"
#include "CombatCommander.h"
#include "TimerManager.h"

namespace UAlbertaBot
{


class UnitToAssign
{
public:

	BWAPI::Unit unit;
	bool isAssigned = false;

	UnitToAssign(BWAPI::Unit u)
        : unit(u)
	{
		
	}
};

class GameCommander 
{
	CombatCommander m_combatCommander;
	TimerManager    m_timerManager;

	BWAPI::Unitset  m_validUnits;
	BWAPI::Unitset  m_combatUnits;
	BWAPI::Unitset  m_scoutUnits;

    bool            m_initialScoutSet = false;

    void assignUnit(BWAPI::Unit unit, BWAPI::Unitset & set);
	bool isAssigned(BWAPI::Unit unit) const;

public:

	GameCommander();

	void update();

	void handleUnitAssignments();
	void setValidUnits();
	void setScoutUnits();
	void setCombatUnits();

	void drawDebugInterface();
    void drawGameInformation(int x, int y);

	BWAPI::Unit getFirstSupplyProvider();
	BWAPI::Unit getClosestUnitToTarget(BWAPI::UnitType type, BWAPI::Position target);
	BWAPI::Unit getClosestWorkerToTarget(BWAPI::Position target);

	void onUnitShow(BWAPI::Unit unit);
	void onUnitHide(BWAPI::Unit unit);
	void onUnitCreate(BWAPI::Unit unit);
	void onUnitComplete(BWAPI::Unit unit);
	void onUnitRenegade(BWAPI::Unit unit);
	void onUnitDestroy(BWAPI::Unit unit);
	void onUnitMorph(BWAPI::Unit unit);
};

}