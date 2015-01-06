#include "UnitCommandData.h"

using namespace UAlbertaBot;

UnitCommandData::UnitCommandData()
	: _unit(NULL)
	, _phase(UnitCommandData::NONE)
	, _enteredReady(0)
	, _enteredAttacking(0)
	, _enteredReloading(0)
	, _waitCommandGiven(0)
{

}

UnitCommandData::UnitCommandData(BWAPI::UnitInterface* unit)
	: _unit(unit)
	, _phase(UnitCommandData::NONE)
	, _enteredReady(0)
	, _enteredAttacking(0)
	, _enteredReloading(0)
	, _waitCommandGiven(0)
{
}

const bool UnitCommandData::isWaiting() const
{
	return (getPhase() == RELOADING) && (_waitCommandGiven > _enteredReloading);
}

const int UnitCommandData::getPhase() const
{
	return _phase;
}

void UnitCommandData::waitCommand() 
{
	_waitCommandGiven = BWAPI::Broodwar->getFrameCount();
}

void UnitCommandData::update()
{
	int currentFrame = BWAPI::Broodwar->getFrameCount();

	if (_phase == NONE)
	{
		if (_unit->getGroundWeaponCooldown() == 0)
		{
			_phase = READY;
			_enteredReady = currentFrame;
		}
		else
		{
			_phase = RELOADING;
			_enteredReloading = currentFrame;
		}
	}
	else if (_phase == READY)
	{
		if (_unit->isAttackFrame() || _unit->isStartingAttack())
		{
			_phase = ATTACKING;
			_enteredAttacking = currentFrame;
		}
	}
	else if (_phase == ATTACKING)
	{
		if (_unit->getGroundWeaponCooldown() == 0)
		{
			_phase = READY;
			_enteredReady = currentFrame;
		}
		else if (!_unit->isAttackFrame() || ((currentFrame - _enteredAttacking) > SparCraft::AnimationFrameData::getAttackFrames(_unit->getType()).first))
		{
			_phase = RELOADING;
			_enteredReloading = currentFrame;
		}
	}
	else if (_phase == RELOADING)
	{
		if (_unit->getGroundWeaponCooldown() == 0)
		{
			_phase = READY;
			_enteredReady = currentFrame;
		}
	}
}
