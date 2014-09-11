#pragma once

#include "Common.h"
#include "BWAPI.h"
#include "UnitCommandData.h"

namespace UAlbertaBot
{

class UnitCommandManager
{
	std::map<BWAPI::Unit *, UnitCommandData> map;

	UnitCommandData & getUnitData(BWAPI::Unit * unit);

	UnitCommandManager();

public:

	static UnitCommandManager &	Instance();

	void update();
	void updateUnit(BWAPI::Unit * unit);
	void addUnit(BWAPI::Unit * unit);
	void removeUnit(BWAPI::Unit * unit);
	void drawDebugPlate(BWAPI::Unit * unit, char * string);
	void drawCommandStatus(BWAPI::Unit * unit);

	bool exists(BWAPI::Unit * unit) const;
	bool commandWillInterruptAttack(BWAPI::Unit * unit);
	bool canIssueAttackCommand(BWAPI::Unit * attacker, BWAPI::Unit * target);
	bool canIssueMoveCommand(BWAPI::Unit * unit, BWAPI::Position & position);
	bool canIssueStopCommand(BWAPI::Unit * unit);
	int attackAnimStartTime(BWAPI::Unit * unit);

	void smartAttackUnit(BWAPI::Unit * attacker, BWAPI::Unit * target);
	void smartMove(BWAPI::Unit * attacker, BWAPI::Position targetPosition);
	void smartReload(BWAPI::Unit * unit);

	void waitCommand(BWAPI::Unit * unit);
	void attackCommand(BWAPI::Unit * unit);
};

}