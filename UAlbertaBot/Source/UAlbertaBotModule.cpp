/* 
 +----------------------------------------------------------------------+
 | UAlbertaBot                                                          |
 +----------------------------------------------------------------------+
 | University of Alberta - AIIDE StarCraft Competition                  |
 +----------------------------------------------------------------------+
 |                                                                      |
 +----------------------------------------------------------------------+
 | Author: David Churchill <dave.churchill@gmail.com>                   |
 +----------------------------------------------------------------------+
*/

#include "Common.h"
#include "UAlbertaBotModule.h"
#include "JSONTools.h"

using namespace UAlbertaBot;

// This gets called when the bot starts!
void UAlbertaBotModule::onStart()
{
    // Initialize SparCraft, the combat simulation package
    SparCraft::init();

    // Initialize BOSS, the Build Order Search System
    BOSS::init();

    // Parse the bot's configuration file if it has one, change this file path to where your config file is
    // Any relative path name will be relative to Starcraft installation folder
    parseConfigFile("D:/ualbertabot/UAlbertaBot/UAlbertaBot_Config.txt");

    // Set our BWAPI options here    
	BWAPI::Broodwar->setLocalSpeed(Config::BWAPIOptions::SetLocalSpeed);
	BWAPI::Broodwar->setFrameSkip(Config::BWAPIOptions::SetFrameSkip);
    
    if (Config::BWAPIOptions::EnableCompleteMapInformation)
    {
        BWAPI::Broodwar->enableFlag(BWAPI::Flag::CompleteMapInformation);
    }

    if (Config::BWAPIOptions::EnableUserInput)
    {
        BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);
    }

    if (Config::BotInfo::PrintInfoOnStart)
    {
        BWAPI::Broodwar->printf("Hello! I am %s, written by %s", Config::BotInfo::BotName.c_str(), Config::BotInfo::Authors.c_str());
    }

    // Call BWTA to read and analyze the current map
    if (Config::Modules::UsingGameCommander)
	{
		BWTA::readMap();
		BWTA::analyze();
	}
}

void UAlbertaBotModule::parseConfigFile(const std::string & filename)
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
        if (info.HasMember("Race") && info["Race"].IsString()) { Config::BotInfo::BotRace = getRace(info["Race"].GetString()); }
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
        JSONTools::ReadBool("PrintModuleTimeout", debug, Config::Debug::PrintModuleTimeout);
        JSONTools::ReadBool("PrintBuildOrderSearchInfo", debug, Config::Debug::PrintBuildOrderSearchInfo);
        JSONTools::ReadBool("DrawResourceInfo", debug, Config::Debug::DrawResourceInfo);
        JSONTools::ReadBool("DrawWorkerInfo", debug, Config::Debug::DrawWorkerInfo);
        JSONTools::ReadBool("DrawBuildOrderInfo", debug, Config::Debug::DrawBuildOrderInfo);
        JSONTools::ReadBool("DrawProductionInfo", debug, Config::Debug::DrawProductionInfo);
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
            Config::Debug::PrintBuildOrderSearchInfo      = false;
            Config::Debug::DrawResourceInfo               = false;
            Config::Debug::DrawBuildOrderInfo             = false;
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
        JSONTools::ReadString("StrategyName", strategy, Config::Strategy::StrategyName);
        BWAPI::Broodwar->printf("Using Strategy: %s", Config::Strategy::StrategyName.c_str());

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
                    strategyRace = getRace(val["Race"].GetString());
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

void UAlbertaBotModule::onEnd(bool isWinner) 
{
	if (Config::Modules::UsingGameCommander)
	{
		StrategyManager::Instance().onEnd(isWinner);
        
		ProductionManager::Instance().onGameEnd();
	}	
}

void UAlbertaBotModule::onFrame()
{
	if (Config::Modules::UsingUnitCommandManager)
	{
		UnitCommandManager::Instance().update();
	}

	if (Config::Modules::UsingGameCommander) 
	{ 
		gameCommander.update(); 
	}
}

void UAlbertaBotModule::onUnitDestroy(BWAPI::UnitInterface* unit)
{
	if (Config::Modules::UsingGameCommander) { gameCommander.onUnitDestroy(unit); }
}

void UAlbertaBotModule::onUnitMorph(BWAPI::UnitInterface* unit)
{
	if (Config::Modules::UsingGameCommander) { gameCommander.onUnitMorph(unit); }
}

void UAlbertaBotModule::onSendText(std::string text) 
{ 
	if (Config::Modules::UsingBuildOrderDemo)
	{
		std::stringstream type;
		std::stringstream numUnitType;
		size_t numUnits = 0;

		size_t i=0;
		for (i=0; i<text.length(); ++i)
		{
			if (text[i] == ' ')
			{
				i++;
				break;
			}

			type << text[i];
		}

		for (; i<text.length(); ++i)
		{
			numUnitType << text[i];
		}

		numUnits = atoi(numUnitType.str().c_str());

        BWAPI::UnitType t;
        for (const BWAPI::UnitType & tt : BWAPI::UnitTypes::allUnitTypes())
        {
            if (tt.getName().compare(type.str()) == 0)
            {
                t = tt;
                break;
            }
        }
	
		BWAPI::Broodwar->printf("Searching for %d of %s", numUnits, t.getName().c_str());

        if (t != BWAPI::UnitType())
        {
            MetaPairVector goal;
		    goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Probe, 8));
		    goal.push_back(MetaPair(BWAPI::UnitTypes::Protoss_Gateway, 2));
		    goal.push_back(MetaPair(t, numUnits));

		    ProductionManager::Instance().setSearchGoal(goal);
        }
        else
        {
            BWAPI::Broodwar->printf("Unknown unit type %s", type.str().c_str());
        }

		
	}
}

void UAlbertaBotModule::onUnitCreate(BWAPI::UnitInterface* unit)
{ 
	if (Config::Modules::UsingGameCommander) { gameCommander.onUnitCreate(unit); }
}

void UAlbertaBotModule::onUnitComplete(BWAPI::UnitInterface* unit)
{
	if (Config::Modules::UsingGameCommander) { gameCommander.onUnitComplete(unit); }
}

void UAlbertaBotModule::onUnitShow(BWAPI::UnitInterface* unit)
{ 
	if (Config::Modules::UsingGameCommander) { gameCommander.onUnitShow(unit); }
}

void UAlbertaBotModule::onUnitHide(BWAPI::UnitInterface* unit)
{ 
	if (Config::Modules::UsingGameCommander) { gameCommander.onUnitHide(unit); }
}

void UAlbertaBotModule::onUnitRenegade(BWAPI::UnitInterface* unit)
{ 
	if (Config::Modules::UsingGameCommander) { gameCommander.onUnitRenegade(unit); }
}

BWAPI::Race UAlbertaBotModule::getRace(const std::string & raceName)
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