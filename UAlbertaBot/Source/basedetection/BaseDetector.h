#pragma once
#include <vector>
#include "../BaseLocation.h"

namespace AKBot
{
	class BaseDetector
	{
	public:
		// Detect bases on the map
		virtual void detectBases(std::vector<UAlbertaBot::BaseLocation>& baseLocations) const = 0;
	};
}