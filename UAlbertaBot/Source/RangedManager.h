#pragma once;

#include <Common.h>
#include "MicroManager.h"

namespace UAlbertaBot
{
class RangedManager : public MicroManager
{
	const BotMicroConfiguration& _microConfiguration;
public:

	RangedManager(
		shared_ptr<AKBot::OpponentView> opponentView,
		shared_ptr<BaseLocationManager> bases,
		const BotMicroConfiguration& microConfiguration);
	void executeMicro(const std::vector<BWAPI::Unit> & targets, int currentFrame);

	int getAttackPriority(BWAPI::Unit rangedUnit, BWAPI::Unit target);
	BWAPI::Unit getTarget(BWAPI::Unit rangedUnit, const std::vector<BWAPI::Unit> & targets);

    void assignTargets(const std::vector<BWAPI::Unit> & targets, int currentFrame);
};
}