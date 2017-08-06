#pragma once
#include "BaseLocationManager.h"
#include "MapTools.h"
#include "ScreenCanvas.h"
#include "OpponentView.h"

namespace AKBot
{
	using UAlbertaBot::BaseLocation;
	using UAlbertaBot::BaseLocationManager;
	using UAlbertaBot::MapTools;

	/*
	 Allow draw debug information about base locations
	*/
	class BaseLocationManagerDebug
	{
		std::shared_ptr<OpponentView> _opponentView;
		shared_ptr<BaseLocationManager> _baseLocationManager;
		shared_ptr<MapTools> _map;
	public:
		BaseLocationManagerDebug(
			std::shared_ptr<OpponentView> opponentView,
			shared_ptr<BaseLocationManager> baseLocationManager,
			shared_ptr<MapTools> map);
		void draw(AKBot::ScreenCanvas& canvas);
		void drawBase(const BaseLocation& base, AKBot::ScreenCanvas& canvas, std::function<bool(BWAPI::TilePosition tile)> isBuildableTile) const;
	};
}