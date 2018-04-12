#pragma once
#include "CommandExecutor.h"
#include "..\MapTools.h"

namespace AKBot
{
	class MapCommandExecutor : public CommandExecutor
	{
		UAlbertaBot::MapTools& _map;
	public:
		MapCommandExecutor(UAlbertaBot::MapTools& map);
		void onCommand(const std::string& command) override;
		bool isSupported(const std::string& command) const override;
	};
}