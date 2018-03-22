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
		void placeTerranMarine(BWAPI::UnitData& unitData, int x, int y);
		
		void placeZergZergeling(BWAPI::UnitData& unitData, int x, int y);
		void placeZergHydralisk(BWAPI::UnitData& unitData, int x, int y);
		void placeZergUltralisk(BWAPI::UnitData& unitData, int x, int y);
		void placeZergSunkenColony(BWAPI::UnitData& unitData, int x, int y);

		void placeProtossZealot(BWAPI::UnitData& unitData, int x, int y);
		
		void placeUnit(BWAPI::UnitData& unitData, BWAPI::UnitType unitType, int x, int y);

		class UnitBuilder
		{
			BWAPI::UnitData& _unitData;
		public:
			UnitBuilder(BWAPI::UnitData& unitData);
			UnitBuilder& unit(BWAPI::UnitType unitType);
			UnitBuilder& position(int x, int y);
			UnitBuilder& position(BWAPI::Position position);
			UnitBuilder& position(BWAPI::TilePosition tilePosition);
			UnitBuilder& angle(double angle);
			UnitBuilder& my();
			UnitBuilder& player(int player);
			UnitBuilder& visibleToAll();
			BWAPI::UnitData& data() const { return _unitData; }
		};
	}
}