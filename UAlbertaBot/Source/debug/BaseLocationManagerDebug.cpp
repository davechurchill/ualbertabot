#pragma once
#include "BaseLocationManagerDebug.h"

namespace AKBot
{
	BaseLocationManagerDebug::BaseLocationManagerDebug(
		std::shared_ptr<OpponentView> opponentView,
		shared_ptr<BaseLocationManager> baseLocationManager,
		shared_ptr<MapTools> map,
		const BotDebugConfiguration& debugConfiguration)
		: _opponentView(opponentView)
		, _baseLocationManager(baseLocationManager)
		, _map(map)
		, _debugConfiguration(debugConfiguration)
	{
	}
	void BaseLocationManagerDebug::draw(AKBot::ScreenCanvas & canvas, int currentFrame)
	{
		drawBases(canvas);
		drawBasesSummary(canvas, 300, 100);
		drawNextExpansion(canvas);
	}

	void BaseLocationManagerDebug::drawBases(AKBot::ScreenCanvas & canvas)
	{
		auto isBuildableTileCheck = [this](BWAPI::TilePosition tile)
		{
			return _map->isBuildableTile(tile);
		};
		for (auto baseLocation : _baseLocationManager->getBaseLocations())
		{
			drawBase(*baseLocation, canvas, isBuildableTileCheck);
		}
	}

	void BaseLocationManagerDebug::drawBasesSummary(AKBot::ScreenCanvas & canvas, int x, int y)
	{
		canvas.drawTextScreen(x, y, "\x03 Id: \x1f Occupied By");
		y += 12;
		for (auto baseLocation : _baseLocationManager->getBaseLocations())
		{
			std::string owned;
			bool firstLocation = true;
			if (baseLocation->isOccupiedByPlayer(_opponentView->self())) {
				if (firstLocation) {
					owned = "me";
					firstLocation = false;
				}
				else {
					owned += ",me";
				}
			}

			for (auto player : _opponentView->enemies()) {

				if (baseLocation->isOccupiedByPlayer(player)) {
					if (firstLocation) {
						owned = player->getName();
						firstLocation = false;
					}
					else {
						owned += ",";
						owned += player->getName();
					}
				}
			}

			canvas.drawTextScreen(x, y, "\x03 %d \x1f %s", baseLocation->getId(), owned.c_str());
			y += 12;
		}
	}

	void BaseLocationManagerDebug::drawNextExpansion(AKBot::ScreenCanvas & canvas)
	{
		auto self = _opponentView->self();
		BWAPI::Position nextExpansionPosition(_baseLocationManager->getNextExpansion(self));

		canvas.drawCircleMap(nextExpansionPosition, 16, BWAPI::Colors::Orange, true);
	}

	void BaseLocationManagerDebug::drawBase(const BaseLocation& base, AKBot::ScreenCanvas& canvas, std::function<bool(BWAPI::TilePosition tile)> isBuildableTile) const
	{
		auto bounds = base.getRecourceBounds();
		auto centerOfResources = BWAPI::Position(
			bounds.left + (bounds.right - bounds.left) / 2,
			bounds.top + (bounds.bottom - bounds.top) / 2);
		canvas.drawCircleMap(centerOfResources, 16, BWAPI::Colors::Yellow);

		std::stringstream ss;
		auto mineralPositions = base.getMineralPositions();
		auto geyserPositions = base.getGeyserPositions();
		ss << "BaseLocation: " << base.getId() << "\n";
		ss << "Start Loc:    " << (base.isStartLocation() ? "true" : "false") << "\n";
		ss << "Minerals:     " << mineralPositions.size() << "\n";
		ss << "Geysers:      " << geyserPositions.size() << "\n";
		ss << "Occupied By:  ";

		if (base.isOccupiedByPlayer(_opponentView->self()))
		{
			ss << "Self ";
		}

		for (auto& enemy : _opponentView->enemies())
		{
			if (base.isOccupiedByPlayer(enemy))
			{
				ss << "Enemy " << enemy->getName() << " ";
				break;
			}
		}

		canvas.drawTextMap(bounds.left, bounds.top - 60, ss.str().c_str());
		canvas.drawTextMap(bounds.left, bounds.bottom, ss.str().c_str());

		// draw the base bounding box
		canvas.drawBoxMap(bounds.left, bounds.top, bounds.right, bounds.bottom, BWAPI::Colors::White, false);
		BWAPI::Position mDiag(BWAPI::UnitTypes::Resource_Mineral_Field.dimensionLeft(),
			BWAPI::UnitTypes::Resource_Mineral_Field.dimensionDown());
		BWAPI::Position gDiag(BWAPI::UnitTypes::Resource_Vespene_Geyser.dimensionLeft(),
			BWAPI::UnitTypes::Resource_Vespene_Geyser.dimensionDown());
		for (auto & mineralPos : mineralPositions)
		{
			canvas.drawBoxMap(mineralPos - mDiag, mineralPos + mDiag, BWAPI::Colors::Cyan, false);
			//canvas.drawLineMap(mineralPos, _resourceCenter, BWAPI::Colors::Cyan);
		}

		for (auto & geyserPos : geyserPositions)
		{
			canvas.drawBoxMap(geyserPos - gDiag, geyserPos + gDiag, BWAPI::Colors::Green, false);
			//canvas.drawLineMap(geyserPos, _resourceCenter, BWAPI::Colors::Green);
		}

		auto position = base.getPosition();
		if (base.isStartLocation())
		{
			canvas.drawCircleMap(position, 10, BWAPI::Colors::Red, true);
		}

		if (_debugConfiguration.DrawBaseTiles)
		{
			auto & closestTiles = base.getClosestTiles();
			for (size_t i = 0; i < 200 && i < closestTiles.size(); ++i)
			{
				const BWAPI::TilePosition & tile = closestTiles[i];

				BWAPI::Position pos = BWAPI::Position(tile) + BWAPI::Position(16, 16);

				if (!pos.isValid())
				{
					continue;
				}

				BWAPI::Color color = isBuildableTile(tile) ? BWAPI::Colors::Green : BWAPI::Colors::Red;
				if (isBuildableTile(tile) && !_map->isDepotBuildableTile(tile))
				{
					color = BWAPI::Colors::Blue;
				}

				if (_map->isBuildable(tile, BWAPI::UnitTypes::Terran_Command_Center))
				{
					color = BWAPI::Colors::Purple;
				}

				canvas.drawCircleMap(pos, 5, color, false);
				canvas.drawTextMap(pos, "%d", base.getGroundTileDistance(pos));
			}
		}

		int ccWidth = BWAPI::UnitTypes::Terran_Command_Center.tileWidth() * 32;
		int ccHeight = BWAPI::UnitTypes::Terran_Command_Center.tileHeight() * 32;
		canvas.drawBoxMap(position, position + BWAPI::Position(ccWidth, ccHeight), BWAPI::Colors::Red, false);
	}
}