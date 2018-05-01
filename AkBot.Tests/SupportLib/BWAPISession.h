#pragma once
#include <BWAPI\Game.h>

namespace AKBot
{
	namespace Tests
	{
		class BWAPISession
		{
		public:
			BWAPISession(BWAPI::Game* game)
			{
				BWAPI::BroodwarPtr = game;
			}
			~BWAPISession() {
				BWAPI::BroodwarPtr = nullptr;
			}
		};
	}
}