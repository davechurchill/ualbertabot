#pragma once

#include "Common.h"

namespace UAlbertaBot
{

class StrategyManager;

namespace ParseUtils
{
	/**
	 * Finds location of the configuration file.
	 */
	std::string FindConfigurationLocation(const std::string & filename);
    void ParseConfigFile(const std::string & filename);
    void ParseStrategy(const std::string & filename, StrategyManager & strategyManager);
    BWAPI::Race GetRace(const std::string & raceName);

    std::string ReadFile(const std::string & filename);
}
}