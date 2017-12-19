#pragma once
#include <BWAPI\Client\UnitData.h>
#include <BWAPI\UnitType.h>

namespace AKBot
{
	namespace Tests
	{
		void placeMineral(BWAPI::UnitData& unitData, int x, int y, int resources);
		void placeGeyser(BWAPI::UnitData& unitData, int x, int y, int resources);
		void placeTerrainBunker(BWAPI::UnitData& unitData, int x, int y);
		void placeTerrainMedic(BWAPI::UnitData& unitData, int x, int y);
		
		void placeZergZergeling(BWAPI::UnitData& unitData, int x, int y);
		void placeZergUltralisk(BWAPI::UnitData& unitData, int x, int y);
		
		void placeUnit(BWAPI::UnitData& unitData, BWAPI::UnitType unitType, int x, int y);
	}
}