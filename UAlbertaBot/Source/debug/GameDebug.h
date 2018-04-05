#pragma once
#include <vector>
#include <memory>
#include "DebugInfoProvider.h"

namespace AKBot
{
	using std::vector;
	using std::shared_ptr;
	using std::unique_ptr;

	class GameDebug
	{
		vector<shared_ptr<DebugInfoProvider>> _debugProviders;
	public:
		GameDebug(vector<shared_ptr<DebugInfoProvider>> debugProviders);
		/*
		 Draw debug information on the screen canvas.
		*/
		void draw(ScreenCanvas& canvas, int currentFrame);
	};
}
