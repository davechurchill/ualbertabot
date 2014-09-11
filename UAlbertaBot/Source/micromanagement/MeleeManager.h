#pragma once;

#include <Common.h>
#include "MicroManager.h"

namespace UAlbertaBot
{
class MicroManager;

class MeleeManager : public MicroManager
{

public:

	MeleeManager();
	~MeleeManager() {}
	void executeMicro(const UnitVector & targets);

	BWAPI::Unit * chooseTarget(BWAPI::Unit * meleeUnit, const UnitVector & targets, std::map<BWAPI::Unit *, int> & numTargeting);
	BWAPI::Unit * closestMeleeUnit(BWAPI::Unit * target, std::set<BWAPI::Unit *> & meleeUnitToAssign);
	int getAttackPriority(BWAPI::Unit * unit);
	BWAPI::Unit * getTarget(BWAPI::Unit * meleeUnit, UnitVector & targets);
};
}