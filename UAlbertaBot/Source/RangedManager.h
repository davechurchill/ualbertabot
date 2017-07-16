#pragma once;

#include <Common.h>
#include "MicroManager.h"

namespace UAlbertaBot
{
class RangedManager : public MicroManager
{
public:

	RangedManager(const AKBot::OpponentView& opponentView, const BaseLocationManager& bases);
	void executeMicro(const std::vector<BWAPI::Unit> & targets, int currentFrame);

	int getAttackPriority(BWAPI::Unit rangedUnit, BWAPI::Unit target);
	BWAPI::Unit getTarget(BWAPI::Unit rangedUnit, const std::vector<BWAPI::Unit> & targets);

    void assignTargets(const std::vector<BWAPI::Unit> & targets, int currentFrame);
};
}