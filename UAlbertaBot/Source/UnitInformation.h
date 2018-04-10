#pragma once
#include <BWAPI/Unitset.h>

namespace AKBot
{
	class UnitInformation
	{
	public:
		virtual const BWAPI::Unitset& getAllUnits() const = 0;
		virtual const BWAPI::Unitset getUnitsOnTile(int tileX, int tileY) const = 0;
	};
}