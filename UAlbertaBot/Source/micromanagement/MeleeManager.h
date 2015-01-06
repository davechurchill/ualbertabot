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
	void executeMicro(const std::vector<BWAPI::UnitInterface *> & targets);

	BWAPI::UnitInterface* chooseTarget(BWAPI::UnitInterface* meleeUnit, const std::vector<BWAPI::UnitInterface *> & targets, std::map<BWAPI::UnitInterface*, int> & numTargeting);
	BWAPI::UnitInterface* closestMeleeUnit(BWAPI::UnitInterface* target, std::set<BWAPI::UnitInterface*> & meleeUnitToAssign);
	int getAttackPriority(BWAPI::UnitInterface* unit);
	BWAPI::UnitInterface* getTarget(BWAPI::UnitInterface* meleeUnit, std::vector<BWAPI::UnitInterface *> & targets);
};
}