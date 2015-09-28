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
	SquadData       _squadData;
    BWAPI::Unitset  _combatUnits;
    bool            _initialized;

    void            updateScoutDefenseSquad();
	void            updateDefenseSquads();
	void            updateAttackSquads();
	void            updateIdleSquad();
	bool            isSquadUpdateFrame();
	int             getNumType(BWAPI::Unitset & units, BWAPI::UnitType type);

	BWAPI::Unit   findClosestDefender(const Squad & defenseSquad, BWAPI::Position pos, bool flyingDefender);
    BWAPI::Unit  findClosestWorkerToTarget(BWAPI::Unitset & unitsToAssign, BWAPI::Unit target);

	BWAPI::Position         getDefendLocation();
    BWAPI::Position         getMainAttackLocation();

    void                    initializeSquads();
    void                    verifySquadUniqueMembership();
    void                    assignFlyingDefender(Squad & squad);
    void                    emptySquad(Squad & squad, BWAPI::Unitset & unitsToAssign);
    int                     getNumGroundDefendersInSquad(Squad & squad);
    int                     getNumAirDefendersInSquad(Squad & squad);

    void                    updateDefenseSquadUnits(Squad & defenseSquad, const size_t & flyingDefendersNeeded, const size_t & groundDefendersNeeded);
    int                     defendWithWorkers();

public:

	CombatCommander();

	void update(const BWAPI::Unitset & combatUnits);
    
	void drawSquadInformation(int x, int y);
};
}