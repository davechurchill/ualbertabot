#pragma once

#include <Common.h>
#include "MicroManager.h"

namespace UAlbertaBot
{
class TankManager : public MicroManager
{
public:

	TankManager();
	void executeMicro(const BWAPI::Unitset & targets);
    
	int getAttackPriority(BWAPI::Unit rangedUnit, BWAPI::Unit target);
	BWAPI::Unit getTarget(BWAPI::Unit rangedUnit, const BWAPI::Unitset & targets);
	BWAPI::Unit closestrangedUnit(BWAPI::Unit target, std::set<BWAPI::Unit> & rangedUnitsToAssign);
};
}