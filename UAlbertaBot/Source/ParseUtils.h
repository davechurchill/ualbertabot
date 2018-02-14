#pragma once
#include <string>
#include <memory>
#include <BWAPI/Race.h>
#include "BotConfiguration.h"
#include "OpponentView.h"

namespace UAlbertaBot
{

class StrategyManager;

namespace ParseUtils
{
	/**
	 * Finds location of the configuration file.
	 */
	std::string FindConfigurationLocation(const std::string & filename);
    void ParseConfigFile(
		const std::string & filename,
		BotConfiguration& config,
		bool& configFileFound,
		bool& configFileParsed);
    void ParseStrategy(
		shared_ptr<AKBot::OpponentView> opponentView,
		BotStrategyConfiguration& strategyOptions,
		shared_ptr<StrategyManager> strategyManager);

    std::string ReadFile(const std::string & filename);
}
}