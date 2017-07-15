#pragma once
#include "MapTools.h"
#include "BaseLocationManager.h"
#include "ScreenCanvas.h"

namespace AKBot
{
	using UAlbertaBot::MapTools;
	using UAlbertaBot::BaseLocationManager;

	class MapToolsDebug
	{
		const MapTools& _map;
		const BaseLocationManager & _bases;
	public:
		MapToolsDebug(const MapTools& map, const BaseLocationManager & bases);
		void drawLastSeen(AKBot::ScreenCanvas& canvas, const BaseLocationManager & bases) const;
	};
}