#include "UnitHelper.h"
#include <BWAPI\UnitType.h>
#include <Windows.h>

using namespace AKBot::Tests;

AKBot::Tests::UnitBuilder::UnitBuilder(BWAPI::UnitData& unitData)
	: _unitData(unitData)
{
	ZeroMemory(&unitData, sizeof(unitData));
}

AKBot::Tests::UnitBuilder::UnitBuilder(BWAPI::UnitData& unitData, int unitId)
	: _unitData(unitData)
{
	ZeroMemory(&unitData, sizeof(unitData));
	unitData.id = unitId;
}

UnitBuilder& UnitBuilder::unit(BWAPI::UnitType unitType)
{
	_unitData.type = unitType;
	_unitData.hitPoints = unitType.maxHitPoints();
	_unitData.shields = unitType.maxShields();
	_unitData.exists = true;
	_unitData.isCompleted = true;
	if (unitType == BWAPI::UnitTypes::Zerg_Broodling)
	{
		_unitData.removeTimer = 4200;
	}

	return *this;
}

UnitBuilder& UnitBuilder::position(int x, int y)
{
	_unitData.positionX = x;
	_unitData.positionY = y;
	return *this;
}

UnitBuilder& UnitBuilder::position(BWAPI::Position position)
{
	_unitData.positionX = position.x;
	_unitData.positionY = position.y;
	return *this;
}

UnitBuilder& UnitBuilder::position(BWAPI::TilePosition tilePosition)
{
	BWAPI::Position position(tilePosition);
	_unitData.positionX = position.x;
	_unitData.positionY = position.y;
	return *this;
}

UnitBuilder& UnitBuilder::angle(double angle)
{
	_unitData.angle = angle;
	return *this;
}

UnitBuilder& UnitBuilder::my()
{
	_unitData.player = 0;
	return *this;
}

UnitBuilder& UnitBuilder::player(int player)
{
	_unitData.player = player;
	return *this;
}

UnitBuilder& UnitBuilder::hidden()
{
	_unitData.hitPoints = 0;
	_unitData.shields = 0;
	return *this;
}

UnitBuilder& UnitBuilder::visibleToAll()
{
	_unitData.isVisible[0] = true;
	_unitData.isVisible[1] = true;
	return *this;
}

UnitBuilder& UnitBuilder::playerVisibility(int playerId, bool visible)
{
	_unitData.isVisible[playerId] = visible;
	return *this;
}

UnitBuilder& UnitBuilder::mineral(int amount)
{
	_unitData.type = BWAPI::UnitTypes::Resource_Mineral_Field;
	_unitData.exists = true;
	_unitData.isCompleted = true;
	_unitData.resources = amount;
	return *this;
}

UnitBuilder& UnitBuilder::geyser(int amount)
{
	_unitData.type = BWAPI::UnitTypes::Resource_Vespene_Geyser;
	_unitData.exists = true;
	_unitData.isCompleted = true;
	_unitData.resources = amount;
	return *this;
}