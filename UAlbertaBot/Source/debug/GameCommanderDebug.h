#pragma once

#include "DebugInfoProvider.h"
#include "GameCommander.h"
#include "ScreenCanvas.h"

namespace AKBot
{
	using UAlbertaBot::GameCommander;

	class GameCommanderDebug : public DebugInfoProvider
	{
		shared_ptr<GameCommander> _gameCommander;

		void drawGameInformation(ScreenCanvas& canvas, int x, int y) const;
	public:
		GameCommanderDebug(shared_ptr<GameCommander> gameCommander);
		void draw(ScreenCanvas& canvas) override;
	};
}
