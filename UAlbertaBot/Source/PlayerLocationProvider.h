#pragma once
#include <memory>
#include <BWAPI/Player.h>
#include "BaseLocation.h"

namespace AKBot
{
	using UAlbertaBot::BaseLocation;

	class PlayerLocationProvider
	{
	public:
		virtual const BaseLocation* getPlayerStartingBaseLocation(BWAPI::Player player) const = 0;
	};
}