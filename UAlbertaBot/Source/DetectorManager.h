#pragma once;

#include <Common.h>
#include "MicroManager.h"
#include "MapTools.h"

namespace UAlbertaBot
{
class MicroManager;

class DetectorManager : public MicroManager
{
	std::map<BWAPI::Unit, bool>	cloakedUnitMap;
	shared_ptr<MapTools> _mapTools;

	bool isAssigned(BWAPI::Unit unit);

	BWAPI::Unit unitClosestToEnemy;

public:

	DetectorManager(shared_ptr<AKBot::OpponentView> opponentView, shared_ptr<MapTools> mapTool, shared_ptr<BaseLocationManager> basess);
	~DetectorManager() {}

	void setUnitClosestToEnemy(BWAPI::Unit unit) { unitClosestToEnemy = unit; }
	void executeMicro(const std::vector<BWAPI::Unit> & targets, int currentFrame);

	BWAPI::Unit closestCloakedUnit(const std::vector<BWAPI::Unit> & cloakedUnits, BWAPI::Unit detectorUnit);
};
}