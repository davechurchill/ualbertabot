#include "Config.h"
#include "UABAssert.h"

using namespace UAlbertaBot;

BotConfiguration Config;

namespace ConfigOld
{
    namespace ConfigFile
    {
        bool ConfigFileFound                = true;
        bool ConfigFileParsed               = true;
        std::string ConfigFileLocation      = "AK_Config.json";
    }
}