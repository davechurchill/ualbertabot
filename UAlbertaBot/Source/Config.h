#pragma once

#include "BotConfiguration.h"

extern BotConfiguration Config;

namespace ConfigOld
{
    namespace ConfigFile
    {
        extern bool ConfigFileFound;
        extern bool ConfigFileParsed;
        extern std::string ConfigFileLocation;
    }
}