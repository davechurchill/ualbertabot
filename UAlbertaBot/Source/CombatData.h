#pragma once

#include "Common.h"
#include "BWAPI.h"
#include "GameCommander.h"
#include "..\..\AdversarialSearch\source\Timer.h"
#include "..\..\AdversarialSearch\source\GameState.h"
#include "..\..\AdversarialSearch\source\Player.h"
#include "..\..\AdversarialSearch\source\TranspositionTable.h"

#include <boost/shared_ptr.hpp>


class CombatUnitData
{	
	BWAPI::Unit * _unit;

	int		_phase,
			_enteredReady,
			_enteredAttacking,
			_enteredReloading,
			_waitCommandGiven;

public:

	enum	{ NONE, READY, ATTACKING, RELOADING };

	CombatUnitData();
	CombatUnitData(BWAPI::Unit * unit);

	void update();
	void waitCommand();
	void attackCommand();
	const bool isWaiting() const;
	const int getPhase() const;
};

class CombatData
{
	std::map<BWAPI::Unit *, CombatUnitData> map;

	CombatUnitData & getUnitData(BWAPI::Unit * unit);

public:

	CombatData();

	void updateUnit(BWAPI::Unit * unit);
	void addUnit(BWAPI::Unit * unit);
	void removeUnit(BWAPI::Unit * unit);
	void drawDebugPlate(BWAPI::Unit * unit, char * string);

	bool exists(BWAPI::Unit * unit) const;
	bool commandWillInterruptAttack(BWAPI::Unit * unit);
	bool canIssueAttackCommand(BWAPI::Unit * attacker, BWAPI::Unit * target);
	bool canIssueMoveCommand(BWAPI::Unit * unit, BWAPI::Position & position);
	bool canIssueStopCommand(BWAPI::Unit * unit);
	int attackAnimStartTime(BWAPI::Unit * unit);

	void waitCommand(BWAPI::Unit * unit);
	void attackCommand(BWAPI::Unit * unit);

	const std::pair<int, int> getUnitCooldown(BWAPI::Unit * unit, MicroSearch::Unit & u) const;
};
