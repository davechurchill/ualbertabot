#include "UnitCommandManager.h"

using namespace UAlbertaBot;

UnitCommandManager::UnitCommandManager() {}

// get an instance of this
UnitCommandManager & UnitCommandManager::Instance() 
{
	static UnitCommandManager instance;
	return instance;
}

void UnitCommandManager::update()
{
	BOOST_FOREACH (BWAPI::Unit * unit, BWAPI::Broodwar->self()->getUnits())
	{
		if (unit->getType().canAttack())
		{
			updateUnit(unit);
		}
	}
}

UnitCommandData & UnitCommandManager::getUnitData(BWAPI::Unit * unit)
{
	return map[unit];
}

void UnitCommandManager::updateUnit(BWAPI::Unit * unit)
{
    if (!unit)
    {
        return;

    }
	// if it's not in our map, add a default entry
	if (!exists(unit))
	{
		addUnit(unit);
	}

	getUnitData(unit).update();
}

void UnitCommandManager::addUnit(BWAPI::Unit * unit)
{
	map[unit] = UnitCommandData(unit);
}

void UnitCommandManager::removeUnit(BWAPI::Unit * unit)
{
	map.erase(unit);
}

bool UnitCommandManager::exists(BWAPI::Unit * unit) const
{
	return map.find(unit) != map.end();
}

bool UnitCommandManager::commandWillInterruptAttack(BWAPI::Unit * unit)
{
	const UnitCommandData & unitData(getUnitData(unit));

	if (unitData.getPhase() == UnitCommandData::ATTACKING)
	{
		return true;
	}

	//BWAPI::Broodwar->drawTextScreen(20, 20, "ATTACK PASS!");

	return false;
}

bool UnitCommandManager::canIssueAttackCommand(BWAPI::Unit * attacker, BWAPI::Unit * target)
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
		
	//return true;
	return !commandWillInterruptAttack(attacker);
}

bool UnitCommandManager::canIssueMoveCommand(BWAPI::Unit * unit, BWAPI::Position & position)
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

	//if (getUnitData(unit).isWaiting())
	//{
	//	drawDebugPlate(unit, "WAIT");
	//	return false;
	//}

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

bool UnitCommandManager::canIssueStopCommand(BWAPI::Unit * unit)
{
	// if the last move was attacking a unit do nothing
	if (unit->getLastCommand().getType() == BWAPI::UnitCommandTypes::Attack_Unit)
	{
		return false;
	}

	return !commandWillInterruptAttack(unit);
}

void UnitCommandManager::drawDebugPlate(BWAPI::Unit * unit, char * string)
{
	BWAPI::Broodwar->drawBoxMap(unit->getPosition().x()-15, unit->getPosition().y()-10, unit->getPosition().x() + 10, unit->getPosition().y(), BWAPI::Colors::Black, true);
	BWAPI::Broodwar->drawTextMap(unit->getPosition().x()-15, unit->getPosition().y()-10, string);
}

void UnitCommandManager::drawCommandStatus(BWAPI::Unit * unit)
{
	BWAPI::UnitType type = unit->getType();
	int width = type.dimensionRight();
	int height = type.dimensionDown()/6;

	int x1 = unit->getPosition().x() - width;
	int x2 = unit->getPosition().y() + width;
	int y1 = unit->getPosition().y() - height;
	int y2 = unit->getPosition().y() + height;
}

void UnitCommandManager::waitCommand(BWAPI::Unit * unit)
{
	getUnitData(unit).waitCommand();
}

void UnitCommandManager::smartReload(BWAPI::Unit * unit)
{
	assert(unit);

	if (!canIssueStopCommand(unit))
	{
		return;
	}

	unit->stop();
	waitCommand(unit);
}

void UnitCommandManager::smartAttackUnit(BWAPI::Unit * attacker, BWAPI::Unit * target)
{
	assert(attacker && target);

	if (!canIssueAttackCommand(attacker, target))
	{
		return;
	}

	attacker->attack(target);

	if (Options::Debug::DRAW_UALBERTABOT_DEBUG) 
	{
		BWAPI::Broodwar->drawLineMap(attacker->getPosition().x()+3, attacker->getPosition().y()+3, target->getPosition().x()+3, target->getPosition().y()+3, BWAPI::Colors::Red );
	}
}

void UnitCommandManager::smartMove(BWAPI::Unit * attacker, BWAPI::Position targetPosition)
{
	assert(attacker);

	if (!canIssueMoveCommand(attacker, targetPosition))
	{
		return;
	}
	
	attacker->move(targetPosition);

	if (Options::Debug::DRAW_UALBERTABOT_DEBUG) 
	{
		BWAPI::Broodwar->drawLineMap(attacker->getPosition().x(), attacker->getPosition().y(), targetPosition.x(), targetPosition.y(), BWAPI::Colors::Orange);
	}
}