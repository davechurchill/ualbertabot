#pragma once 
#include "..\ScreenCanvas.h"

namespace AKBot
{
	class DebugInfoProvider
	{
	public:
		virtual ~DebugInfoProvider() = default;
		/*
		 Draw debug information on the screen canvas.
		*/
		virtual void draw(ScreenCanvas& canvas, int currentFrame) = 0;
	};
}