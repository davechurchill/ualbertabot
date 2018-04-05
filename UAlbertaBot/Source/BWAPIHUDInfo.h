#pragma once
#include "HUDInfo.h"
#include <BWAPI/Game.h>

namespace AKBot
{
	class BWAPIHUDInfo : public HUDInfo
	{
		BWAPI::Game* _game;
	public:
		BWAPIHUDInfo(BWAPI::Game* game);
		BWAPI::Position getMousePosition() const override;
		BWAPI::Position getScreenPosition() const override;
		bool getMouseState(BWAPI::MouseButton button) const override;
	};
}