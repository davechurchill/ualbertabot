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
    void ParseTextCommand(const std::string & commandLine);
    BWAPI::Race GetRace(const std::string & raceName);

    int GetIntFromString(const std::string & str);
    bool GetBoolFromString(const std::string & str);

    std::string ReadFile(const std::string & filename);
}
}