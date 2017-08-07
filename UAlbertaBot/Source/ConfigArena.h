#pragma once

#include "BWAPI.h"
#include <cassert>

namespace ConfigArena
{
    namespace ConfigFile
    {
        extern bool ConfigFileFound;
        extern bool ConfigFileParsed;
        extern std::string ConfigFileLocation;
    }

    namespace BotInfo
    {
        extern std::string BotName;
        extern std::string Authors;
        extern bool PrintInfoOnStart;
    }

    namespace BWAPIOptions
    {
        extern int SetLocalSpeed;
        extern int SetFrameSkip;
        extern bool EnableUserInput;
        extern bool EnableCompleteMapInformation;
    }

    namespace Debug
    {
        extern bool DrawGameInfo;
        extern bool DrawUnitHealthBars;
        extern bool DrawModuleTimers;
        extern bool DrawMouseCursorInfo;

        extern BWAPI::Color ColorLineTarget;
        extern BWAPI::Color ColorLineMineral;
        extern BWAPI::Color ColorUnitNearEnemy;
        extern BWAPI::Color ColorUnitNotNearEnemy;
    }

    namespace SparCraftOptions
    {
        extern std::string SparCraftConfigFile;
        extern std::string ArenaSparCraftPlayerName;
        extern bool SparCraftConfigFileFound;
        extern bool SparCraftConfigFileParsed;
    }

    void ParseArenaConfigFile(const std::string & filename);
    std::string ReadFile(const std::string & filename);
}
