#include "CombatData.h"

CombatUnitData::CombatUnitData()
	: _unit(NULL)
	, _phase(CombatUnitData::NONE)
	, _enteredReady(0)
	, _enteredAttacking(0)
	, _enteredReloading(0)
	, _waitCommandGiven(0)
{

}

CombatUnitData::CombatUnitData(BWAPI::Unit * unit)
	: _unit(unit)
	, _phase(CombatUnitData::NONE)
	, _enteredReady(0)
	, _enteredAttacking(0)
	, _enteredReloading(0)
	, _waitCommandGiven(0)
{
}

const bool CombatUnitData::isWaiting() const
{
	return (getPhase() == RELOADING) && (_waitCommandGiven > _enteredReloading);
}

const int CombatUnitData::getPhase() const
{
	return _phase;
}

void CombatUnitData::waitCommand() 
{
	_waitCommandGiven = BWAPI::Broodwar->getFrameCount();
}

void CombatUnitData::update()
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
		else if (!_unit->isAttackFrame() || ((currentFrame - _enteredAttacking) > Search::StarcraftData::getAttackFrames(_unit->getType()).first))
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

CombatData::CombatData() {}

CombatUnitData & CombatData::getUnitData(BWAPI::Unit * unit)
{
	return map[unit];
}

void CombatData::updateUnit(BWAPI::Unit * unit)
{
	// if it's not in our map, add a default entry
	if (!exists(unit))
	{
		addUnit(unit);
	}

	getUnitData(unit).update();
}

void CombatData::addUnit(BWAPI::Unit * unit)
{
	map[unit] = CombatUnitData(unit);
}

void CombatData::removeUnit(BWAPI::Unit * unit)
{
	map.erase(unit);
}

bool CombatData::exists(BWAPI::Unit * unit) const
{
	return map.find(unit) != map.end();
}

bool CombatData::commandWillInterruptAttack(BWAPI::Unit * unit)
{
	const CombatUnitData & unitData(getUnitData(unit));

	if (unitData.getPhase() == CombatUnitData::ATTACKING)
	{
		return true;
	}

	BWAPI::Broodwar->drawTextScreen(20, 20, "ATTACK PASS!");

	return false;
}

bool CombatData::canIssueAttackCommand(BWAPI::Unit * attacker, BWAPI::Unit * target)
{
	BWAPI::UnitCommand currentCommand(attacker->getLastCommand());
	BWAPI::UnitCommandType commandType = currentCommand.getType();

	// if we have already given a command this frame, don't issue another one
	//if (commandType != BWAPI::UnitCommandTypes::None && BWAPI::Broodwar->getFrameCount() - attacker->getLastCommandFrame() <= 7)
	//{
	//	drawDebugPlate(attacker, "A FRAME");
	//	return false;
	//}

	// if the last command given was an attack command
	if (currentCommand.getType() == BWAPI::UnitCommandTypes::Attack_Unit)
	{
		// if the target is the same as the current one, we don't need to switch
		if (currentCommand.getTarget() == target)
		{
			drawDebugPlate(attacker, "SAME");
			return false;
		}
	}
		
	return true;
	//return !commandWillInterruptAttack(attacker);
}

bool CombatData::canIssueMoveCommand(BWAPI::Unit * unit, BWAPI::Position & position)
{
	BWAPI::UnitCommand currentCommand(unit->getLastCommand());
	BWAPI::UnitCommandType commandType = currentCommand.getType();

	int threshold = 5;

	// if we have already given a command this frame, don't issue another one
	//if (commandType != BWAPI::UnitCommandTypes::None && BWAPI::Broodwar->getFrameCount() - unit->getLastCommandFrame() <= 7)
	//{
	//	drawDebugPlate(unit, "M FRAME");
	//	return false;
	//}

	if (getUnitData(unit).isWaiting())
	{
		drawDebugPlate(unit, "WAIT");
		return false;
	}

	// if the last command given was an attack command
	if (currentCommand.type == BWAPI::UnitCommandTypes::Move)
	{
		if (!unit->isMoving())
		{
			return true;
		}
		else
		{
			return unit->getDistance(position) < 28;
		}
	}

	return !commandWillInterruptAttack(unit);
}

bool CombatData::canIssueStopCommand(BWAPI::Unit * unit)
{
	// if the last move was attacking a unit do nothing
	if (unit->getLastCommand().getType() == BWAPI::UnitCommandTypes::Attack_Unit)
	{
		return false;
	}

	return !commandWillInterruptAttack(unit);
}

const std::pair<int, int> CombatData::getUnitCooldown(BWAPI::Unit * unit, MicroSearch::Unit & u) const
{
	int attackCooldown(0);
	int moveCooldown(0);

	BWAPI::UnitCommand lastCommand = unit->getLastCommand();
	int lastCommandFrame = unit->getLastCommandFrame();
	int currentFrame = BWAPI::Broodwar->getFrameCount();
	int framesSinceCommand = currentFrame - lastCommandFrame;

	
	if ((unit->getType() == BWAPI::UnitTypes::Protoss_Dragoon) && (unit->getLastCommand().getType() == BWAPI::UnitCommandTypes::Attack_Unit))
	{
		// dragoons are one of only 2 unit types whose attack can be canceled by the in-game targeter being called too early so
		// this hack makes up for that by taking it's stop-delay into account
		attackCooldown = BWAPI::Broodwar->getFrameCount() + std::max(0, unit->getGroundWeaponCooldown()-Search::StarcraftData::getAttackFrames(unit->getType()).first);
	}
	else
	{
		attackCooldown = BWAPI::Broodwar->getFrameCount() + std::max(0, unit->getGroundWeaponCooldown()-2);
	}

	// if the last attack was an attack command
	if (lastCommand.getType() == BWAPI::UnitCommandTypes::Attack_Unit)
	{
		moveCooldown = BWAPI::Broodwar->getFrameCount() + std::max(0, u.attackInitFrameTime() - framesSinceCommand);

		//BWAPI::Broodwar->drawTextScreen(100,100, "%d, %d", attackCooldown-currentFrame, moveCooldown-currentFrame);
	}
	// if the last command was a move command
	else if (lastCommand.getType() == BWAPI::UnitCommandTypes::Move)
	{
		moveCooldown = currentFrame;
	}

	if (moveCooldown - BWAPI::Broodwar->getFrameCount() < 4 || unit->isMoving())
	{
		moveCooldown = currentFrame;
	}

	moveCooldown = std::min(moveCooldown, attackCooldown);

	return std::pair<int, int>(attackCooldown, moveCooldown);
}

void CombatData::drawDebugPlate(BWAPI::Unit * unit, char * string)
{
	BWAPI::Broodwar->drawBoxMap(unit->getPosition().x()-15, unit->getPosition().y()-10, unit->getPosition().x() + 10, unit->getPosition().y(), BWAPI::Colors::Black, true);
	BWAPI::Broodwar->drawTextMap(unit->getPosition().x()-15, unit->getPosition().y()-10, string);
}

void CombatData::waitCommand(BWAPI::Unit * unit)
{
	getUnitData(unit).waitCommand();
}
