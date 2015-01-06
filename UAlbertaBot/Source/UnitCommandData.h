#pragma once

#include "Common.h"
#include "BWAPI.h"

#include "..\..\SparCraft\source\SparCraft.h"

namespace UAlbertaBot
{
class UnitCommandData
{	
	BWAPI::UnitInterface* _unit;

	int		_phase,
			_enteredReady,
			_enteredAttacking,
			_enteredReloading,
			_waitCommandGiven;

public:

	enum	{ NONE, READY, ATTACKING, RELOADING };

	UnitCommandData();
	UnitCommandData(BWAPI::UnitInterface* unit);

	void update();
	void waitCommand();
	void attackCommand();
	const bool isWaiting() const;
	const int getPhase() const;
};
}