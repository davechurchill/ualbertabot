#pragma once;

#include <Common.h>
#include "MicroManager.h"

namespace UAlbertaBot
{
class MicroManager;

class DetectorManager : public MicroManager
{

	std::map<BWAPI::UnitInterface*, bool>	cloakedUnitMap;

	bool isAssigned(BWAPI::UnitInterface* unit);

	BWAPI::UnitInterface* unitClosestToEnemy;

public:

	DetectorManager();
	~DetectorManager() {}

	void setUnitClosestToEnemy(BWAPI::UnitInterface* unit) { unitClosestToEnemy = unit; }
	void executeMicro(const std::vector<BWAPI::UnitInterface *> & targets);

	BWAPI::UnitInterface* closestCloakedUnit(const std::vector<BWAPI::UnitInterface *> & cloakedUnits, BWAPI::UnitInterface* detectorUnit);

	BWAPI::UnitInterface* chooseTarget(BWAPI::UnitInterface* rangedUnit, const std::vector<BWAPI::UnitInterface *> & targets, std::map<BWAPI::UnitInterface*, int> & numTargeting);
	BWAPI::UnitInterface* closestrangedUnit(BWAPI::UnitInterface* target, std::set<BWAPI::UnitInterface*> & rangedUnitsToAssign);

	int getAttackPriority(BWAPI::UnitInterface* rangedUnit, BWAPI::UnitInterface* target);
	BWAPI::UnitInterface* getTarget(BWAPI::UnitInterface* rangedUnit, std::vector<BWAPI::UnitInterface *> & targets);

	void kiteTarget(BWAPI::UnitInterface* rangedUnit, BWAPI::UnitInterface* target);
	void spellAttackUnit(BWAPI::UnitInterface* detector, BWAPI::UnitInterface* target);
};
}