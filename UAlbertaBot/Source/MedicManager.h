#pragma once;

#include <Common.h>
#include "MicroManager.h"

namespace UAlbertaBot
{
class MedicManager : public MicroManager
{
public:
	MedicManager(shared_ptr<AKBot::OpponentView> opponentView, shared_ptr<BaseLocationManager> bases);
	void executeMicro(const std::vector<BWAPI::Unit> & targets, int currentFrame);
};
}