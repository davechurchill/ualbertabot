#pragma once
#include "PlayerLocationProvider.h"
#include "BaseLocationManager.h"

namespace AKBot
{
	using UAlbertaBot::BaseLocationManager;
	using UAlbertaBot::BaseLocation;

	class DefaultPlayerLocationProvider
		: public PlayerLocationProvider
	{
		const BaseLocationManager* _baseLocationManager;
	public:
		DefaultPlayerLocationProvider(const BaseLocationManager* baseLocationManager);
		virtual const BaseLocation* getPlayerStartingBaseLocation(BWAPI::Player player) override;
	};
};
