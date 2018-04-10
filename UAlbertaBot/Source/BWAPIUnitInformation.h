#pragma once
#include "UnitInformation.h"
#include <BWAPI/Game.h>

namespace AKBot
{
	class BWAPIUnitInformation : public UnitInformation
	{
		BWAPI::Game* _game;
	public:
		BWAPIUnitInformation(BWAPI::Game* game);
		const BWAPI::Unitset& getAllUnits() const override;
		const BWAPI::Unitset getUnitsOnTile(int tileX, int tileY) const override;
	};
}