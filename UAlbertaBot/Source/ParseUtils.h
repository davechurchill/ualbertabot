#pragma once

#include "Common.h"

namespace UAlbertaBot
{
namespace ParseUtils
{
    void ParseConfigFile(const std::string & filename);
    void ParseTextCommand(const std::string & commandLine);
    BWAPI::Race GetRace(const std::string & raceName);

    int GetIntFromString(const std::string & str);
    bool GetBoolFromString(const std::string & str);







}
}