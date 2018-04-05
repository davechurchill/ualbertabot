#pragma once
#include <BWAPI/Unitset.h>

namespace AKBot
{
	class UnitInformation
	{
	public:
		virtual const BWAPI::Unitset& getAllUnits() const = 0;
	};
}