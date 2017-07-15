#pragma once

#include "GameCommander.h"
#include "ScreenCanvas.h"

namespace AKBot
{
	using UAlbertaBot::GameCommander;

	class GameCommanderDebug
	{
		const GameCommander& _gameCommander;

		void drawGameInformation(ScreenCanvas& canvas, int x, int y) const;
	public:
		GameCommanderDebug(const GameCommander& gameCommander);
		void draw(ScreenCanvas& canvas) const;
	};
}
