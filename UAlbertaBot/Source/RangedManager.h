#pragma once;

#include <Common.h>
#include "MicroManager.h"

namespace UAlbertaBot
{
class RangedManager : public MicroManager
{
public:

	RangedManager();
	~RangedManager() {}
	void executeMicro(const BWAPI::Unitset & targets);

	BWAPI::UnitInterface* chooseTarget(BWAPI::UnitInterface* rangedUnit, const BWAPI::Unitset & targets, std::map<BWAPI::UnitInterface*, int> & numTargeting);
	BWAPI::UnitInterface* closestrangedUnit(BWAPI::UnitInterface* target, std::set<BWAPI::UnitInterface*> & rangedUnitsToAssign);

	int getAttackPriority(BWAPI::UnitInterface* rangedUnit, BWAPI::UnitInterface* target);
	BWAPI::UnitInterface* getTarget(BWAPI::UnitInterface* rangedUnit, const BWAPI::Unitset & targets);

	void kiteTarget(BWAPI::UnitInterface* rangedUnit, BWAPI::UnitInterface* target);
};
}