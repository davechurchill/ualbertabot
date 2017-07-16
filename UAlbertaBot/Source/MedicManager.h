#pragma once;

#include <Common.h>
#include "MicroManager.h"

namespace UAlbertaBot
{
class MedicManager : public MicroManager
{
public:
	MedicManager(const AKBot::OpponentView& opponentView, const BaseLocationManager& bases);
	void executeMicro(const std::vector<BWAPI::Unit> & targets, int currentFrame);
};
}