#pragma once;

#include <Common.h>
#include "MicroManager.h"

namespace UAlbertaBot
{
class TankManager : public MicroManager
{
public:

	TankManager(const AKBot::OpponentView& opponentView, const BaseLocationManager& bases);
	void executeMicro(const std::vector<BWAPI::Unit> & targets, int currentFrame);

	BWAPI::Unit chooseTarget(BWAPI::Unit rangedUnit, const std::vector<BWAPI::Unit> & targets, std::map<BWAPI::Unit, int> & numTargeting);
	BWAPI::Unit closestrangedUnit(BWAPI::Unit target, std::set<BWAPI::Unit> & rangedUnitsToAssign);

	int getAttackPriority(BWAPI::Unit rangedUnit, BWAPI::Unit target);
	BWAPI::Unit getTarget(BWAPI::Unit rangedUnit, const std::vector<BWAPI::Unit> & targets);
};
}