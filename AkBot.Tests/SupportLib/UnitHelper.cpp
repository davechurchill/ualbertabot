#include "UnitHelper.h"
#include <BWAPI\UnitType.h>

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
	unitData.positionX = x;
	unitData.positionY = y;
	unitData.type = unitType;
	unitData.hitPoints = unitType.maxHitPoints();
	unitData.shields = unitType.maxShields();
	unitData.exists = true;
	unitData.isCompleted = true;
}