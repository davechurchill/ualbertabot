#include "UnitHelper.h"
#include <BWAPI\UnitType.h>
#include <Windows.h>

using namespace AKBot::Tests;

void AKBot::Tests::placeMineral(BWAPI::UnitData& unitData, int x, int y, int resources)
{
	unitData.positionX = x;
	unitData.positionY = y;
	unitData.type = BWAPI::UnitTypes::Resource_Mineral_Field;
	unitData.exists = true;
	unitData.resources = resources;
}
void AKBot::Tests::placeGeyser(BWAPI::UnitData& unitData, int x, int y, int resources)
{
	unitData.positionX = x;
	unitData.positionY = y;
	unitData.type = BWAPI::UnitTypes::Resource_Vespene_Geyser;
	unitData.exists = true;
	unitData.isCompleted = true;
	unitData.resources = resources;
}
void AKBot::Tests::placeTerrainBunker(BWAPI::UnitData& unitData, int x, int y)
{
	placeUnit(unitData, BWAPI::UnitTypes::Terran_Bunker, x, y);
}
void AKBot::Tests::placeTerrainMedic(BWAPI::UnitData& unitData, int x, int y)
{
	placeUnit(unitData, BWAPI::UnitTypes::Terran_Medic, x, y);
}
void AKBot::Tests::placeTerranMarine(BWAPI::UnitData& unitData, int x, int y)
{
	placeUnit(unitData, BWAPI::UnitTypes::Terran_Marine, x, y);
}
void AKBot::Tests::placeZergZergeling(BWAPI::UnitData& unitData, int x, int y)
{
	placeUnit(unitData, BWAPI::UnitTypes::Zerg_Zergling, x, y);
}
void AKBot::Tests::placeZergHydralisk(BWAPI::UnitData& unitData, int x, int y)
{
	placeUnit(unitData, BWAPI::UnitTypes::Zerg_Hydralisk, x, y);
}
void AKBot::Tests::placeZergUltralisk(BWAPI::UnitData& unitData, int x, int y)
{
	placeUnit(unitData, BWAPI::UnitTypes::Zerg_Ultralisk, x, y);
}
void AKBot::Tests::placeZergSunkenColony(BWAPI::UnitData& unitData, int x, int y)
{
	placeUnit(unitData, BWAPI::UnitTypes::Zerg_Sunken_Colony, x, y);
}
void AKBot::Tests::placeProtossZealot(BWAPI::UnitData& unitData, int x, int y)
{
	placeUnit(unitData, BWAPI::UnitTypes::Protoss_Zealot, x, y);
}
void AKBot::Tests::placeUnit(BWAPI::UnitData& unitData, BWAPI::UnitType unitType, int x, int y)
{
	UnitBuilder builder(unitData);
	builder.unit(unitType).position(x, y);
}

AKBot::Tests::UnitBuilder::UnitBuilder(BWAPI::UnitData& unitData)
	: _unitData(unitData)
{
	ZeroMemory(&unitData, sizeof(unitData));
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

UnitBuilder& UnitBuilder::visibleToAll()
{
	_unitData.isVisible[0] = true;
	_unitData.isVisible[1] = true;
	return *this;
}