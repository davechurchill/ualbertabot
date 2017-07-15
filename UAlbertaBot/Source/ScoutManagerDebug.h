#pragma once
#include "ScoutManager.h"
#include "ScreenCanvas.h"

namespace AKBot
{
	using UAlbertaBot::ScoutManager;

	class ScoutManagerDebug
	{
		const ScoutManager& _scoutManager;
	public:
		ScoutManagerDebug(const ScoutManager& scoutManager);
		void drawScoutInformation(AKBot::ScreenCanvas& canvas, int x, int y) const;
	};
}