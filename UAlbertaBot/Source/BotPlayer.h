#pragma once

#include "Common.h"
#include "BotModule.h"

namespace AKBot
{
	using UAlbertaBot::BotModule;

	class BotPlayer
	{
		shared_ptr<BotModule> m;
	public:
		BotPlayer(unique_ptr<BotModule> _m) : m(std::move(_m)) {}

		shared_ptr<BotModule> getBot() { return m; }

		bool isValid() const { return !!m.get(); }

		void PlayGame();
	};
}
