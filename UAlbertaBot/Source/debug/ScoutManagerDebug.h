#pragma once
#include "..\ScoutManager.h"
#include "..\ScreenCanvas.h"

namespace AKBot
{
	using UAlbertaBot::ScoutManager;

	class ScoutManagerDebug
	{
		shared_ptr<ScoutManager> _scoutManager;
	public:
		ScoutManagerDebug(shared_ptr<ScoutManager> scoutManager);
		void drawScoutInformation(AKBot::ScreenCanvas& canvas, int x, int y) const;
	};
}