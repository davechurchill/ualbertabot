#pragma once
#include <functional>
#include <BWAPI\Unit.h>

namespace AKBot
{
	typedef std::function<void(const BWAPI::Unit& unit)> UnitHandler;
}