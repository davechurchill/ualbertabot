#pragma once;

#include <Common.h>
#include "MicroManager.h"
#include "BotConfiguration.h"

namespace UAlbertaBot
{
class TankManager : public MicroManager
{
public:

	TankManager(
		shared_ptr<AKBot::OpponentView> opponentView,
		shared_ptr<BaseLocationManager> bases,
		const BotMicroConfiguration& microConfiguration);

	void executeMicro(const std::vector<BWAPI::Unit> & targets, int currentFrame);

	BWAPI::Unit chooseTarget(BWAPI::Unit rangedUnit, const std::vector<BWAPI::Unit> & targets, std::map<BWAPI::Unit, int> & numTargeting);
	BWAPI::Unit closestrangedUnit(BWAPI::Unit target, std::set<BWAPI::Unit> & rangedUnitsToAssign);

	int getAttackPriority(BWAPI::Unit rangedUnit, BWAPI::Unit target);
	BWAPI::Unit getTarget(BWAPI::Unit rangedUnit, const std::vector<BWAPI::Unit> & targets);
};
}