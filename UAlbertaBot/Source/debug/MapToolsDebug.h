#pragma once
#include "..\MapTools.h"
#include "..\BaseLocationManager.h"
#include "..\ScreenCanvas.h"
#include "DebugInfoProvider.h"
#include "..\BotConfiguration.h"

namespace AKBot
{
	using UAlbertaBot::MapTools;
	using UAlbertaBot::BaseLocationManager;

	class MapToolsDebug : public DebugInfoProvider
	{
		shared_ptr<MapTools> _map;
		shared_ptr<BaseLocationManager> _bases;
		const BotDebugConfiguration& _debugConfiguration;
	public:
		MapToolsDebug(
			shared_ptr<MapTools> map,
			shared_ptr<BaseLocationManager> bases,
			const BotDebugConfiguration& debugConfiguration);
		MapToolsDebug(const MapToolsDebug&) = delete;
		void draw(AKBot::ScreenCanvas& canvas) override;
	};
}