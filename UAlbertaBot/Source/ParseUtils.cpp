#include "ParseUtils.h"
#include "rapidjson\document.h"
#include "JSONTools.h"
#include "BuildOrder.h"
#include "StrategyManager.h"

using namespace UAlbertaBot;

void ParseUtils::ParseConfigFile(const std::string & filename)
{
    rapidjson::Document doc;

    std::string config = FileUtils::ReadFile(filename);

    if (config.length() == 0)
    {
        UAB_ASSERT_WARNING(config.length() > 0, "Couldn't find any configuration options, using defaults");
        return;
    }

    bool parsingFailed = doc.Parse(config.c_str()).HasParseError();
    if (parsingFailed)
    {
        UAB_ASSERT_WARNING(!parsingFailed, "Parsing of configuration file failed, please check that it's valid JSON format");
        return;
    }

    // Parse the Bot Info
    if (doc.HasMember("Bot Info") && doc["Bot Info"].IsObject())
    {
        const rapidjson::Value & info = doc["Bot Info"];
        JSONTools::ReadString("BotName", info, Config::BotInfo::BotName);
        JSONTools::ReadString("Authors", info, Config::BotInfo::Authors);
        JSONTools::ReadBool("PrintInfoOnStart", info, Config::BotInfo::PrintInfoOnStart);
    }

    // Parse the BWAPI Options
    if (doc.HasMember("BWAPI") && doc["BWAPI"].IsObject())
    {
        const rapidjson::Value & bwapi = doc["BWAPI"];
        JSONTools::ReadInt("SetLocalSpeed", bwapi, Config::BWAPIOptions::SetLocalSpeed);
        JSONTools::ReadInt("SetFrameSkip", bwapi, Config::BWAPIOptions::SetFrameSkip);
        JSONTools::ReadBool("EnableUserInput", bwapi, Config::BWAPIOptions::EnableUserInput);
        JSONTools::ReadBool("EnableCompleteMapInformation", bwapi, Config::BWAPIOptions::EnableCompleteMapInformation);
    }

    // Parse the Micro Options
    if (doc.HasMember("Micro") && doc["Micro"].IsObject())
    {
        const rapidjson::Value & micro = doc["Micro"];
        JSONTools::ReadBool("UseSparcraftSimulation", micro, Config::Micro::UseSparcraftSimulation);
        JSONTools::ReadBool("WorkerDefense", micro, Config::Micro::WorkerDefense);
        JSONTools::ReadInt("WorkerDefensePerUnit", micro, Config::Micro::WorkerDefensePerUnit);
        JSONTools::ReadInt("InCombatRadius", micro, Config::Micro::CombatRadius);
        JSONTools::ReadInt("RegroupRadius", micro, Config::Micro::CombatRegroupRadius);
        JSONTools::ReadInt("UnitNearEnemyRadius", micro, Config::Micro::UnitNearEnemyRadius);
    }

    // Parse the Macro Options
    if (doc.HasMember("Macro") && doc["Macro"].IsObject())
    {
        const rapidjson::Value & macro = doc["Macro"];
        JSONTools::ReadInt("BuildingSpacing", macro, Config::Macro::BuildingSpacing);
        JSONTools::ReadInt("PylongSpacing", macro, Config::Macro::PylonSpacing);
    }

    // Parse the Debug Options
    if (doc.HasMember("Debug") && doc["Debug"].IsObject())
    {
        const rapidjson::Value & debug = doc["Debug"];
        JSONTools::ReadString("ErrorLogFilename", debug, Config::Debug::ErrorLogFilename);
        JSONTools::ReadBool("LogAssertToErrorFile", debug, Config::Debug::LogAssertToErrorFile);
        JSONTools::ReadBool("PrintModuleTimeout", debug, Config::Debug::PrintModuleTimeout);
        JSONTools::ReadBool("DrawBuildOrderSearchInfo", debug, Config::Debug::DrawBuildOrderSearchInfo);
        JSONTools::ReadBool("DrawUnitHealthBars", debug, Config::Debug::DrawUnitHealthBars);
        JSONTools::ReadBool("DrawResourceInfo", debug, Config::Debug::DrawResourceInfo);
        JSONTools::ReadBool("DrawWorkerInfo", debug, Config::Debug::DrawWorkerInfo);
        JSONTools::ReadBool("DrawProductionInfo", debug, Config::Debug::DrawProductionInfo);
        JSONTools::ReadBool("DrawScoutInfo", debug, Config::Debug::DrawScoutInfo);
        JSONTools::ReadBool("DrawSquadInfo", debug, Config::Debug::DrawSquadInfo);
        JSONTools::ReadBool("DrawCombatSimInfo", debug, Config::Debug::DrawCombatSimulationInfo);
        JSONTools::ReadBool("DrawBuildingInfo", debug, Config::Debug::DrawBuildingInfo);
        JSONTools::ReadBool("DrawModuleTimers", debug, Config::Debug::DrawModuleTimers);
        JSONTools::ReadBool("DrawMouseCursorInfo", debug, Config::Debug::DrawMouseCursorInfo);
        JSONTools::ReadBool("DrawEnemyUnitInfo", debug, Config::Debug::DrawEnemyUnitInfo);
        JSONTools::ReadBool("DrawBWTAInfo", debug, Config::Debug::DrawBWTAInfo);
        JSONTools::ReadBool("DrawMapGrid", debug, Config::Debug::DrawMapGrid);
        JSONTools::ReadBool("DrawUnitTargetInfo", debug, Config::Debug::DrawUnitTargetInfo);
        JSONTools::ReadBool("DrawReservedBuildingTiles", debug, Config::Debug::DrawReservedBuildingTiles);

        JSONTools::ReadBool("AllDebugOff", debug, Config::Debug::AllDebugOff);

        if (Config::Debug::AllDebugOff)
        {
            Config::Debug::PrintModuleTimeout             = false;	
            Config::Debug::DrawBuildOrderSearchInfo       = false;
            Config::Debug::DrawUnitHealthBars             = false;
            Config::Debug::DrawResourceInfo               = false;
            Config::Debug::DrawProductionInfo             = false;
            Config::Debug::DrawWorkerInfo                 = false;
            Config::Debug::DrawModuleTimers               = false;
            Config::Debug::DrawReservedBuildingTiles      = false;
            Config::Debug::DrawCombatSimulationInfo       = false;
            Config::Debug::DrawBuildingInfo               = false;
            Config::Debug::DrawMouseCursorInfo            = false;
            Config::Debug::DrawEnemyUnitInfo              = false;
            Config::Debug::DrawBWTAInfo                   = false;
            Config::Debug::DrawMapGrid                    = false;
            Config::Debug::DrawUnitTargetInfo             = false;
            Config::Debug::DrawSquadInfo                  = false;
        }
    }

    // Parse the Module Options
    if (doc.HasMember("Modules") && doc["Modules"].IsObject())
    {
        const rapidjson::Value & module = doc["Modules"];

        JSONTools::ReadBool("UseGameCommander", module, Config::Modules::UsingGameCommander);
        JSONTools::ReadBool("UseScoutManager", module, Config::Modules::UsingScoutManager);
        JSONTools::ReadBool("UseCombatCommander", module, Config::Modules::UsingCombatCommander);
        JSONTools::ReadBool("UseBuildOrderSearch", module, Config::Modules::UsingBuildOrderSearch);
        JSONTools::ReadBool("UseStrategyIO", module, Config::Modules::UsingStrategyIO);
        JSONTools::ReadBool("UseUnitCommandManager", module, Config::Modules::UsingUnitCommandManager);
        JSONTools::ReadBool("UseAutoObserver", module, Config::Modules::UsingAutoObserver);
    }

    // Parse the Tool Options
    if (doc.HasMember("Tools") && doc["Tools"].IsObject())
    {
        const rapidjson::Value & tool = doc["Tools"];

        JSONTools::ReadInt("MapGridSize", tool, Config::Tools::MAP_GRID_SIZE);
    }

    // Parse the Strategy Options
    if (doc.HasMember("Strategy") && doc["Strategy"].IsObject())
    {
        const rapidjson::Value & strategy = doc["Strategy"];

        JSONTools::ReadBool("ScoutGasSteal", strategy, Config::Strategy::GasStealWithScout);
        JSONTools::ReadBool("ScoutHarassEnemy", strategy, Config::Strategy::ScoutHarassEnemy);
        JSONTools::ReadString("ProtossStrategyName", strategy, Config::Strategy::ProtossStrategyName);
        JSONTools::ReadString("TerranStrategyName", strategy, Config::Strategy::TerranStrategyName);
        JSONTools::ReadString("ZergStrategyName", strategy, Config::Strategy::ZergStrategyName);

        BWAPI::Race race = BWAPI::Broodwar->self()->getRace();

        if (race == BWAPI::Races::Protoss)
        {
            Config::Strategy::StrategyName = Config::Strategy::ProtossStrategyName;
        }
        else if (race == BWAPI::Races::Terran)
        {
            Config::Strategy::StrategyName = Config::Strategy::TerranStrategyName;
        }
        else if (race == BWAPI::Races::Zerg)
        {
            Config::Strategy::StrategyName = Config::Strategy::ZergStrategyName;
        }

        BWAPI::Broodwar->printf("Race is %s, using Strategy: %s", BWAPI::Broodwar->self()->getRace().getName().c_str(), Config::Strategy::StrategyName.c_str());

        JSONTools::ReadString("ReadDirectory", strategy, Config::Strategy::ReadDir);
        JSONTools::ReadString("WriteDirectory", strategy, Config::Strategy::WriteDir);

        // Parse all the Strategies
        if (strategy.HasMember("Strategies") && strategy["Strategies"].IsObject())
        {
            const rapidjson::Value & strategies = strategy["Strategies"];
            for (rapidjson::Value::ConstMemberIterator itr = strategies.MemberBegin(); itr != strategies.MemberEnd(); ++itr)
            {
                const std::string &         name = itr->name.GetString();
                const rapidjson::Value &    val  = itr->value;
        
                BWAPI::Race strategyRace;
                if (val.HasMember("Race") && val["Race"].IsString())
                {
                    strategyRace = GetRace(val["Race"].GetString());
                }
                else
                {
                    UAB_ASSERT_WARNING(false, "Strategy must have a Race string. Skipping strategy %s", name.c_str());
                    continue;
                }

                BuildOrder buildOrder(strategyRace);
                if (val.HasMember("OpeningBuildOrder") && val["OpeningBuildOrder"].IsArray())
                {
                    const rapidjson::Value & build = val["OpeningBuildOrder"];

                    for (size_t b(0); b < build.Size(); ++b)
                    {
                        if (build[b].IsString())
                        {
                            MetaType type(build[b].GetString());

                            if (type.getRace() != BWAPI::Races::None)
                            {
                                buildOrder.add(type);
                            }
                        }
                        else
                        {
                            UAB_ASSERT_WARNING(false, "Build order item must be a string %s", name.c_str());
                            continue;
                        }
                    }
                }

                StrategyManager::Instance().addOpeningBuildOrder(name, buildOrder);
            }
        }
    }
}


void ParseUtils::ParseTextCommand(const std::string & commandString)
{
    std::stringstream ss(commandString);

    std::string command;
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    std::string variableName;
    std::transform(variableName.begin(), variableName.end(), variableName.begin(), ::tolower);

    std::string val;

    ss >> command;
    ss >> variableName;
    ss >> val;

    if (command == "/set")
    {
        // BWAPI options
        if (variableName == "setlocalspeed") { Config::BWAPIOptions::SetLocalSpeed = GetIntFromString(val); BWAPI::Broodwar->setLocalSpeed(Config::BWAPIOptions::SetLocalSpeed); }
        else if (variableName == "setframeskip") { Config::BWAPIOptions::SetFrameSkip = GetIntFromString(val); BWAPI::Broodwar->setFrameSkip(Config::BWAPIOptions::SetFrameSkip); }
        else if (variableName == "enableuserinput") { Config::BWAPIOptions::EnableUserInput = GetBoolFromString(val); if (Config::BWAPIOptions::EnableUserInput) BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput); }
        else if (variableName == "enablecompletemapinformation") { Config::BWAPIOptions::EnableCompleteMapInformation = GetBoolFromString(val); if (Config::BWAPIOptions::EnableCompleteMapInformation) BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput); }
        
        // Micro Options
        else if (variableName == "usesparcraftsimulation") { Config::Micro::UseSparcraftSimulation = GetBoolFromString(val); }
        else if (variableName == "workerdefense") { Config::Micro::WorkerDefense = GetBoolFromString(val); }
        else if (variableName == "workerdefenseperunit") { Config::Micro::WorkerDefensePerUnit = GetIntFromString(val); }
        else if (variableName == "incombatradius") { Config::Micro::CombatRadius = GetIntFromString(val); }
        else if (variableName == "regroupradius") { Config::Micro::CombatRegroupRadius = GetIntFromString(val); }
        else if (variableName == "unitnearenemyradius") { Config::Micro::UnitNearEnemyRadius = GetIntFromString(val); }

        // Macro Options
        else if (variableName == "buildingspacing") { Config::Macro::BuildingSpacing = GetIntFromString(val); }
        else if (variableName == "pylonspacing") { Config::Macro::PylonSpacing = GetIntFromString(val); }

        // Debug Options
        else if (variableName == "errorlogfilename") { Config::Debug::ErrorLogFilename = val; }
        else if (variableName == "printmoduletimeout") { Config::Debug::PrintModuleTimeout = GetBoolFromString(val); }
        else if (variableName == "drawbuildordersearchinfo") { Config::Debug::DrawBuildOrderSearchInfo = GetBoolFromString(val); }
        else if (variableName == "drawunithealthbars") { Config::Debug::DrawUnitHealthBars = GetBoolFromString(val); }
        else if (variableName == "drawproductioninfo") { Config::Debug::DrawProductionInfo = GetBoolFromString(val); }
        else if (variableName == "drawenemyunitinfo") { Config::Debug::DrawEnemyUnitInfo = GetBoolFromString(val); }
        else if (variableName == "drawmoduletimers") { Config::Debug::DrawModuleTimers = GetBoolFromString(val); }
        else if (variableName == "drawresourceinfo") { Config::Debug::DrawResourceInfo = GetBoolFromString(val); }
        else if (variableName == "drawcombatsiminfo") { Config::Debug::DrawCombatSimulationInfo = GetBoolFromString(val); }
        else if (variableName == "drawunittargetinfo") { Config::Debug::DrawUnitTargetInfo = GetBoolFromString(val); }
        else if (variableName == "drawbwtainfo") { Config::Debug::DrawBWTAInfo = GetBoolFromString(val); }
        else if (variableName == "drawmapgrid") { Config::Debug::DrawMapGrid = GetBoolFromString(val); }
        else if (variableName == "drawsquadinfo") { Config::Debug::DrawSquadInfo = GetBoolFromString(val); }
        else if (variableName == "drawworkerinfo") { Config::Debug::DrawWorkerInfo = GetBoolFromString(val); }
        else if (variableName == "drawmousecursorinfo") { Config::Debug::DrawMouseCursorInfo = GetBoolFromString(val); }
        else if (variableName == "drawbuildinginfo") { Config::Debug::DrawBuildingInfo = GetBoolFromString(val); }
        else if (variableName == "drawreservedbuildingtiles") { Config::Debug::DrawReservedBuildingTiles = GetBoolFromString(val); }

        // Module Options
        else if (variableName == "usegamecommander") { Config::Modules::UsingGameCommander = GetBoolFromString(val); }
        else if (variableName == "usescoutmanager") { Config::Modules::UsingScoutManager = GetBoolFromString(val); }
        else if (variableName == "usecombatcommander") { Config::Modules::UsingCombatCommander = GetBoolFromString(val); }
        else if (variableName == "usebuildordersearch") { Config::Modules::UsingBuildOrderSearch = GetBoolFromString(val); }
        else if (variableName == "useautoobserver") { Config::Modules::UsingAutoObserver = GetBoolFromString(val); }
        else if (variableName == "usestrategyio") { Config::Modules::UsingStrategyIO = GetBoolFromString(val); }
        else if (variableName == "useunitcommandmanager") { Config::Modules::UsingUnitCommandManager = GetBoolFromString(val); }

        else { UAB_ASSERT_WARNING(false, "Unknown variable name for /set: %s", variableName.c_str()); }
    }
    else
    {
        UAB_ASSERT_WARNING(false, "Unknown command: %s", command.c_str());
    }
}

BWAPI::Race ParseUtils::GetRace(const std::string & raceName)
{
    if (raceName == "Protoss")
    {
        return BWAPI::Races::Protoss;
    }

    if (raceName == "Terran")
    {
        return BWAPI::Races::Terran;
    }

    if (raceName == "Zerg")
    {
        return BWAPI::Races::Zerg;
    }

    if (raceName == "Random")
    {
        return BWAPI::Races::Random;
    }

    UAB_ASSERT_WARNING(false, "Race not found: %s", raceName.c_str());
    return BWAPI::Races::None;
}

int ParseUtils::GetIntFromString(const std::string & str)
{
    std::stringstream ss(str);
    int a = 0;
    ss >> a;
    return a;
}

bool ParseUtils::GetBoolFromString(const std::string & str)
{
    std::string boolStr(str);
    std::transform(boolStr.begin(), boolStr.end(), boolStr.begin(), ::tolower);

    if (boolStr == "true" || boolStr == "t")
    {
        return true;
    }
    else if (boolStr == "false" || boolStr == "f")
    {
        return false;
    }
    else
    {
        UAB_ASSERT_WARNING(false, "Unknown bool from string: %s", str.c_str());
    }

    return false;
}