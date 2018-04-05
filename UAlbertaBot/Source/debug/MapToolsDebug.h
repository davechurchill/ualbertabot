#pragma once
#include "..\MapTools.h"
#include "..\BaseLocationManager.h"
#include "..\ScreenCanvas.h"
#include "DebugInfoProvider.h"
#include "..\BotConfiguration.h"
#include "..\HUDInfo.h"

namespace AKBot
{
	using UAlbertaBot::MapTools;
	using UAlbertaBot::BaseLocationManager;

	class MapToolsDebug : public DebugInfoProvider
	{
		shared_ptr<MapTools> _map;
		shared_ptr<BaseLocationManager> _bases;
		shared_ptr<OpponentView> _opponentView;
		shared_ptr<HUDInfo> _hudInfo;
		const BotDebugConfiguration& _debugConfiguration;
	public:
		MapToolsDebug(
			shared_ptr<OpponentView> opponentView,
			shared_ptr<MapTools> map,
			shared_ptr<BaseLocationManager> bases,
			shared_ptr<HUDInfo> hudInfo,
			const BotDebugConfiguration& debugConfiguration);
		MapToolsDebug(const MapToolsDebug&) = delete;
		void draw(AKBot::ScreenCanvas& canvas, int currentFrame) override;
	};
}