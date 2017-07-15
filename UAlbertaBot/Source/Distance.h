#pragma once
#include <functional>
#include <BWAPI/Position.h>

namespace AKBot
{
	typedef std::function<int(const BWAPI::Position & src, const BWAPI::Position & dest)> DistanceFunction;
}