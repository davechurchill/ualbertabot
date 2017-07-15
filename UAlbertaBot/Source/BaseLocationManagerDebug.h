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
		const OpponentView& _opponentView;
		const BaseLocationManager& _baseLocationManager;
		const MapTools& _map;
	public:
		BaseLocationManagerDebug(const OpponentView& opponentView, const BaseLocationManager& baseLocationManager, const MapTools& map);
		void draw(AKBot::ScreenCanvas& canvas);
		void drawBase(const BaseLocation& base, AKBot::ScreenCanvas& canvas, std::function<bool(BWAPI::TilePosition tile)> isBuildableTile) const;
	};
}