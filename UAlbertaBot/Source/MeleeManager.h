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
	void executeMicro(const BWAPI::Unitset & targets);

	BWAPI::UnitInterface* chooseTarget(BWAPI::UnitInterface* meleeUnit, const BWAPI::Unitset & targets, std::map<BWAPI::UnitInterface*, int> & numTargeting);
	BWAPI::UnitInterface* closestMeleeUnit(BWAPI::UnitInterface* target, const BWAPI::Unitset & meleeUnitToAssign);
	int getAttackPriority(BWAPI::UnitInterface* unit);
	BWAPI::UnitInterface* getTarget(BWAPI::UnitInterface* meleeUnit, const BWAPI::Unitset & targets);
};
}