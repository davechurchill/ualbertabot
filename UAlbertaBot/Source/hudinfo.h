#pragma once
#include <BWAPI\Position.h>
#include <BWAPI\Input.h>

namespace AKBot
{
	/*
	* Information about Game UI HUD
	*/
	class HUDInfo
	{
	public:
		virtual BWAPI::Position getMousePosition() const = 0;
		virtual BWAPI::Position getScreenPosition() const = 0;
		virtual bool getMouseState(BWAPI::MouseButton button) const = 0;
	};
}