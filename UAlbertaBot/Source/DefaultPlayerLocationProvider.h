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
		shared_ptr<BaseLocationManager> _baseLocationManager;
	public:
		DefaultPlayerLocationProvider(shared_ptr<BaseLocationManager> baseLocationManager);
		virtual const BaseLocation* getPlayerStartingBaseLocation(BWAPI::Player player) const override;
	};
};
