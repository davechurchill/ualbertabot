#pragma once
#include "MapTools.h"
#include "BaseLocationManager.h"
#include "ScreenCanvas.h"
#include "DebugInfoProvider.h"

namespace AKBot
{
	using UAlbertaBot::MapTools;
	using UAlbertaBot::BaseLocationManager;

	class MapToolsDebug : public DebugInfoProvider
	{
		shared_ptr<MapTools> _map;
		shared_ptr<BaseLocationManager> _bases;
	public:
		MapToolsDebug(shared_ptr<MapTools> map, shared_ptr<BaseLocationManager> bases);
		void draw(AKBot::ScreenCanvas& canvas) override;
	};
}