#pragma once
#include <memory>
#include <BWAPI/Game.h>
#include "BaseDetector.h"
#include "../MapTools.h"
#include "../OpponentView.h"

namespace AKBot
{
	class ClusterBaseDetector : public BaseDetector
	{
		std::shared_ptr<UAlbertaBot::MapTools> _map;
		std::shared_ptr<AKBot::OpponentView> _opponentView;
		BWAPI::Game* _game;
	public:
		ClusterBaseDetector(
			BWAPI::Game* game,
			std::shared_ptr<AKBot::OpponentView> opponentView, 
			std::shared_ptr<UAlbertaBot::MapTools> map);
		void detectBases(std::vector<UAlbertaBot::BaseLocation>& baseLocations) const override;
	};
}