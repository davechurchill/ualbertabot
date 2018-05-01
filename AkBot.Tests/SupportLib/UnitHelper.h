#pragma once
#include <BWAPI\Client\UnitData.h>
#include <BWAPI\Client\GameData.h>
#include <BWAPI\UnitType.h>

namespace AKBot
{
	namespace Tests
	{			
		class UnitBuilder
		{
			BWAPI::UnitData& _unitData;
		public:
			UnitBuilder(BWAPI::UnitData& unitData);
			UnitBuilder(BWAPI::UnitData& unitData, int unitId);
			UnitBuilder& unit(BWAPI::UnitType unitType);
			UnitBuilder& position(int x, int y);
			UnitBuilder& position(BWAPI::Position position);
			UnitBuilder& position(BWAPI::TilePosition tilePosition);
			UnitBuilder& hidden();
			UnitBuilder& angle(double angle);
			UnitBuilder& my();
			UnitBuilder& player(int player);
			UnitBuilder& visibleToAll();
			UnitBuilder& playerVisibility(int playerId, bool visible);
			UnitBuilder& mineral(int amount);
			UnitBuilder& geyser(int amount);
			BWAPI::UnitData& data() const { return _unitData; }
		};
	}
}