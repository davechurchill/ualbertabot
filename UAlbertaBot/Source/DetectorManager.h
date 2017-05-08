#pragma once;

#include <Common.h>
#include "MicroManager.h"

namespace UAlbertaBot
{
class MicroManager;

class DetectorManager : public MicroManager
{

	std::map<BWAPI::Unit, bool>	cloakedUnitMap;

	bool isAssigned(BWAPI::Unit unit);

	BWAPI::Unit unitClosestToEnemy;

public:

	DetectorManager();
	~DetectorManager() {}

	void setUnitClosestToEnemy(BWAPI::Unit unit) { unitClosestToEnemy = unit; }
	void executeMicro(const std::vector<BWAPI::Unit> & targets);

	BWAPI::Unit closestCloakedUnit(const std::vector<BWAPI::Unit> & cloakedUnits, BWAPI::Unit detectorUnit);
};
}