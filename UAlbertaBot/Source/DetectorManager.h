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
	void executeMicro(const BWAPI::Unitset & targets);

	BWAPI::UnitInterface* closestCloakedUnit(const BWAPI::Unitset & cloakedUnits, BWAPI::UnitInterface* detectorUnit);
};
}