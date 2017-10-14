#include "ConfigArena.h"
#include "UABAssert.h"
#include "JSONTools.h"

using namespace UAlbertaBot;

namespace ConfigArena
{
namespace ConfigFile
{
    bool ConfigFileFound = false;
    bool ConfigFileParsed = false;
    std::string ConfigFileLocation = "bwapi-data/AI/UAlbertaBot_Config.txt";
}

namespace BotInfo
{
    std::string BotName = "UAlbertaBot Arena";
    std::string Authors = "Dave Churchill";
    bool PrintInfoOnStart = false;
}

namespace BWAPIOptions
{
    int SetLocalSpeed = 42;
    int SetFrameSkip = 0;
    bool EnableUserInput = true;
    bool EnableCompleteMapInformation = false;
}

namespace Debug
{
    bool DrawGameInfo = true;
    bool DrawUnitHealthBars = true;
    bool DrawModuleTimers = false;
    bool DrawMouseCursorInfo = false;
}

namespace SparCraftOptions
{
    std::string SparCraftConfigFile = "bwapi-data/AI/SparCraft_Config.txt";
    std::string ArenaSparCraftPlayerName;
}

void ParseArenaConfigFile(const std::string & filename)
{
    rapidjson::Document doc;
    BWAPI::Race race = BWAPI::Broodwar->self()->getRace();
    const char * ourRace = race.getName().c_str();

    std::string config = ReadFile(filename);

    if (config.length() == 0)
    {
        return;
    }

    ConfigArena::ConfigFile::ConfigFileFound = true;

    bool parsingFailed = doc.Parse(config.c_str()).HasParseError();
    if (parsingFailed)
    {
        return;
    }

    // Parse the Bot Info
    if (doc.HasMember("Bot Info") && doc["Bot Info"].IsObject())
    {
        const rapidjson::Value & info = doc["Bot Info"];
        JSONTools::ReadString("BotName", info, ConfigArena::BotInfo::BotName);
        JSONTools::ReadString("Authors", info, ConfigArena::BotInfo::Authors);
        JSONTools::ReadBool("PrintInfoOnStart", info, ConfigArena::BotInfo::PrintInfoOnStart);
    }

    // Parse the BWAPI Options
    if (doc.HasMember("BWAPI") && doc["BWAPI"].IsObject())
    {
        const rapidjson::Value & bwapi = doc["BWAPI"];
        JSONTools::ReadInt("SetLocalSpeed", bwapi, ConfigArena::BWAPIOptions::SetLocalSpeed);
        JSONTools::ReadInt("SetFrameSkip", bwapi, ConfigArena::BWAPIOptions::SetFrameSkip);
        JSONTools::ReadBool("UserInput", bwapi, ConfigArena::BWAPIOptions::EnableUserInput);
        JSONTools::ReadBool("CompleteMapInformation", bwapi, ConfigArena::BWAPIOptions::EnableCompleteMapInformation);
    }

    // Parse the Debug Options
    if (doc.HasMember("Debug") && doc["Debug"].IsObject())
    {
        const rapidjson::Value & debug = doc["Debug"];
        JSONTools::ReadBool("DrawGameInfo", debug, ConfigArena::Debug::DrawGameInfo);
        JSONTools::ReadBool("DrawUnitHealthBars", debug, ConfigArena::Debug::DrawUnitHealthBars);
        JSONTools::ReadBool("DrawModuleTimers", debug, ConfigArena::Debug::DrawModuleTimers);
        JSONTools::ReadBool("DrawMouseCursorInfo", debug, ConfigArena::Debug::DrawMouseCursorInfo);
    }

    ConfigArena::ConfigFile::ConfigFileParsed = true;
}

std::string ReadFile(const std::string & filename)
{
    std::stringstream ss;

    FILE *file = fopen(filename.c_str(), "r");
    if (file != nullptr)
    {
        char line[4096]; /* or other suitable maximum line size */
        while (fgets(line, sizeof line, file) != nullptr) /* read a line */
        {
            ss << line;
        }
        fclose(file);
    }
    else
    {
        BWAPI::Broodwar->printf("Could not open file: %s", filename.c_str());
    }

    return ss.str();
}

}