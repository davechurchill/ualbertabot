#pragma once
#include "..\BaseLocationManager.h"
#include "..\MapTools.h"
#include "..\ScreenCanvas.h"
#include "..\OpponentView.h"
#include "DebugInfoProvider.h"
#include "..\BotConfiguration.h"

namespace AKBot
{
	using UAlbertaBot::BaseLocation;
	using UAlbertaBot::BaseLocationManager;
	using UAlbertaBot::MapTools;

	/*
	 Allow draw debug information about base locations
	*/
	class BaseLocationManagerDebug: public DebugInfoProvider
	{
		std::shared_ptr<OpponentView> _opponentView;
		shared_ptr<BaseLocationManager> _baseLocationManager;
		shared_ptr<MapTools> _map;
		const BotDebugConfiguration& _debugConfiguration;
	public:
		BaseLocationManagerDebug(
			shared_ptr<OpponentView> opponentView,
			shared_ptr<BaseLocationManager> baseLocationManager,
			shared_ptr<MapTools> map,
			const BotDebugConfiguration& debugConfiguration);
		void draw(AKBot::ScreenCanvas& canvas, int currentFrame) override;
		void drawNextExpansion(AKBot::ScreenCanvas & canvas);
		void drawBases(AKBot::ScreenCanvas & canvas);
		void drawBasesSummary(AKBot::ScreenCanvas & canvas, int x, int y);
		void drawBase(const BaseLocation& base, AKBot::ScreenCanvas& canvas, std::function<bool(BWAPI::TilePosition tile)> isBuildableTile) const;
	};
}