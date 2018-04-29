#pragma once

#include "Common.h"
#include "MetaType.h"
#include "DistanceMap.h"
#include "BaseLocation.h"
#include "Distance.h"
#include "UnitInfoManager.h"
#include "basedetection\BaseDetector.h"
#include "BotConfiguration.h"

namespace AKBot
{
	struct BaseDetectorEntry
	{
		std::unique_ptr<AKBot::BaseDetector> baseDetector;
	};
}

namespace UAlbertaBot
{
	/*
		Manage location of the player bases.
	*/
	class BaseLocationManager
	{
		BWAPI::Game* _game;
		const BotBaseDetectionConfiguration& _baseDetectionConfiguration;
		std::vector<BaseLocation>   _baseLocationData;
		std::shared_ptr<AKBot::OpponentView> _opponentView;

		std::vector<const BaseLocation *> _baseLocationPtrs;
		std::vector<const BaseLocation *> _startingBaseLocations;
		std::map<BWAPI::Player, const BaseLocation *> _playerStartingBaseLocations;
		std::map<BWAPI::Player, std::set<const BaseLocation *>> _occupiedBaseLocations;
		std::map<std::string, AKBot::BaseDetectorEntry> _registry;

		std::vector<std::vector<BaseLocation *>> _tileBaseLocations;  // precomputed base locations for each tile on the map, nullptr if not in a base

		BaseLocation * _getBaseLocation(BWAPI::TilePosition pos) const;
		BaseLocation * _getBaseLocation(BWAPI::Position pos) const;

		/*
			Reset the player occupation information for each location
		*/
		void resetPlayerOccupation();

		const AKBot::BaseDetectorEntry& getBaseDetectorEntry();
	public:
		BaseLocationManager(
			BWAPI::Game* game,
			std::shared_ptr<AKBot::OpponentView> opponentView,
			const BotBaseDetectionConfiguration& baseDetectionConfiguration);

		void update(shared_ptr<UnitInfoManager> unitManager);
		void onStart(shared_ptr<MapTools> map);

		const std::vector<const BaseLocation *> & getBaseLocations() const;
		const std::vector<const BaseLocation *> & getStartingBaseLocations() const;
		const std::set<const BaseLocation *> & getOccupiedBaseLocations(BWAPI::Player player) const;
		const BaseLocation * getPlayerStartingBaseLocation(BWAPI::Player player) const;
		BWAPI::TilePosition getNextExpansion(BWAPI::Player player) const;

		const BaseLocation * getBaseLocation(BWAPI::TilePosition pos) const;
		const BaseLocation * getBaseLocation(BWAPI::Position pos) const;
		BWAPI::Position         getLeastRecentlySeenPosition(shared_ptr<MapTools> bases) const;

		// Gets name of the current base detecor name
		const std::string getBaseDetector() const { return _baseDetectionConfiguration.BaseDetectionStrategy; }
		AKBot::BaseDetector& getCurrentBaseDetector();
		void registerBaseDetector(std::string name, std::unique_ptr<AKBot::BaseDetector> detector);
	};

}