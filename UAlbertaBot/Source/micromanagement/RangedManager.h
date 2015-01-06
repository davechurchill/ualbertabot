#pragma once;

#include <Common.h>
#include "MicroManager.h"

namespace UAlbertaBot
{
class MicroManager;

class RangedManager : public MicroManager
{
public:

	RangedManager();
	~RangedManager() {}
	void executeMicro(const std::vector<BWAPI::UnitInterface *> & targets);

	BWAPI::UnitInterface* chooseTarget(BWAPI::UnitInterface* rangedUnit, const std::vector<BWAPI::UnitInterface *> & targets, std::map<BWAPI::UnitInterface*, int> & numTargeting);
	BWAPI::UnitInterface* closestrangedUnit(BWAPI::UnitInterface* target, std::set<BWAPI::UnitInterface*> & rangedUnitsToAssign);

	int getAttackPriority(BWAPI::UnitInterface* rangedUnit, BWAPI::UnitInterface* target);
	BWAPI::UnitInterface* getTarget(BWAPI::UnitInterface* rangedUnit, std::vector<BWAPI::UnitInterface *> & targets);

	void kiteTarget(BWAPI::UnitInterface* rangedUnit, BWAPI::UnitInterface* target);
};
}