#pragma once

#include "Common.h"
#include "BWAPI.h"
#include "UnitCommandData.h"

namespace UAlbertaBot
{

class UnitCommandManager
{
	std::map<BWAPI::UnitInterface*, UnitCommandData> map;

	UnitCommandData & getUnitData(BWAPI::UnitInterface* unit);

	UnitCommandManager();

public:

	static UnitCommandManager &	Instance();

	void update();
	void updateUnit(BWAPI::UnitInterface* unit);
	void addUnit(BWAPI::UnitInterface* unit);
	void removeUnit(BWAPI::UnitInterface* unit);
	void drawDebugPlate(BWAPI::UnitInterface* unit, char * string);
	void drawCommandStatus(BWAPI::UnitInterface* unit);

	bool exists(BWAPI::UnitInterface* unit) const;
	bool commandWillInterruptAttack(BWAPI::UnitInterface* unit);
	bool canIssueAttackCommand(BWAPI::UnitInterface* attacker, BWAPI::UnitInterface* target);
	bool canIssueMoveCommand(BWAPI::UnitInterface* unit, BWAPI::Position & position);
	bool canIssueStopCommand(BWAPI::UnitInterface* unit);
	int attackAnimStartTime(BWAPI::UnitInterface* unit);

	void smartAttackUnit(BWAPI::UnitInterface* attacker, BWAPI::UnitInterface* target);
	void smartMove(BWAPI::UnitInterface* attacker, BWAPI::Position targetPosition);
	void smartReload(BWAPI::UnitInterface* unit);

	void waitCommand(BWAPI::UnitInterface* unit);
	void attackCommand(BWAPI::UnitInterface* unit);
};

}