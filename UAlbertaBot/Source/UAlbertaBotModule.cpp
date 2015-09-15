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

using namespace UAlbertaBot;

// This gets called when the bot starts!
void UAlbertaBotModule::onStart()
{
    // Initialize SparCraft, the combat simulation package
    SparCraft::init();

    // Initialize BOSS, the Build Order Search System
    BOSS::init();

    // Parse the bot's configuration file if it has one
    parseConfigFile("D:/ualbertabot/UAlbertaBot/UAlbertaBot_Config.txt");

    // Set our BWAPI options here    
	BWAPI::Broodwar->setLocalSpeed(Options::BWAPIOptions::SetLocalSpeed);
	BWAPI::Broodwar->setFrameSkip(Options::BWAPIOptions::SetFrameSkip);
    
    if (Options::BWAPIOptions::EnableCompleteMapInformation)
    {
        BWAPI::Broodwar->enableFlag(BWAPI::Flag::CompleteMapInformation);
    }

    if (Options::BWAPIOptions::EnableUserInput)
    {
        BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);
    }
	
    // Call BWTA to read and analyze the current map
    if (Options::Modules::USING_GAMECOMMANDER)
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
        if (info.HasMember("BotName") && info["BotName"].IsString()) { Options::BotInfo::BotName = info["BotName"].GetString(); }
        if (info.HasMember("Authors") && info["Authors"].IsString()) { Options::BotInfo::Authors = info["Authors"].GetString(); }
        if (info.HasMember("Race") && info["Race"].IsString()) { Options::BotInfo::BotRace = getRace(info["Race"].GetString()); }
        if (info.HasMember("PrintInfoOnStart") && info["PrintInfoOnStart"].IsBool()) { Options::BotInfo::PrintInfoOnStart = info["PrintInfoOnStart"].GetBool(); }
    }

    // Parse the BWAPI Options
    if (doc.HasMember("BWAPI Options") && doc["BWAPI Options"].IsObject())
    {
        const rapidjson::Value & bwapi = doc["BWAPI Options"];
        if (bwapi.HasMember("SetLocalSpeed") && bwapi["SetLocalSpeed"].IsInt()) { Options::BWAPIOptions::SetLocalSpeed = bwapi["SetLocalSpeed"].GetInt(); }
        if (bwapi.HasMember("SetFrameSkip") && bwapi["SetFrameSkip"].IsInt()) { Options::BWAPIOptions::SetFrameSkip = bwapi["SetFrameSkip"].GetInt(); }
        if (bwapi.HasMember("EnableUserInput") && bwapi["EnableUserInput"].IsBool()) { Options::BWAPIOptions::EnableUserInput = bwapi["EnableUserInput"].GetBool(); }
        if (bwapi.HasMember("EnableCompleteMapInformation") && bwapi["EnableCompleteMapInformation"].IsBool()) { Options::BWAPIOptions::EnableCompleteMapInformation = bwapi["EnableCompleteMapInformation"].GetBool(); }
    }

    // Parse the Micro Options
    if (doc.HasMember("Micro Options") && doc["Micro Options"].IsObject())
    {
        const rapidjson::Value & micro = doc["Micro Options"];
        if (micro.HasMember("WorkerDefense") && micro["WorkerDefense"].IsBool()) { Options::Micro::WORKER_DEFENSE = micro["WorkerDefense"].GetBool(); }
        if (micro.HasMember("WorkerDefensePerUnit") && micro["WorkerDefensePerUnit"].IsInt()) { Options::Micro::WORKER_DEFENSE_PER_UNIT = micro["WorkerDefensePerUnit"].GetInt(); }
        if (micro.HasMember("InCombatRadius") && micro["InCombatRadius"].IsInt()) { Options::Micro::COMBAT_RADIUS = micro["InCombatRadius"].GetInt(); }
        if (micro.HasMember("RegroupRadius") && micro["RegroupRadius"].IsInt()) { Options::Micro::COMBAT_REGROUP_RADIUS = micro["RegroupRadius"].GetInt(); }
        if (micro.HasMember("UnitNearEnemyRadius") && micro["UnitNearEnemyRadius"].IsInt()) { Options::Micro::UNIT_NEAR_ENEMY_RADIUS = micro["UnitNearEnemyRadius"].GetInt(); }
    }

    // Parse the Debug Options
    if (doc.HasMember("Debug Options") && doc["Debug Options"].IsObject())
    {
        const rapidjson::Value & debug = doc["Debug Options"];
        if (debug.HasMember("DrawDebugInformation") && debug["DrawDebugInformation"].IsBool()) { Options::Debug::DRAW_DEBUG_INTERFACE = debug["DrawDebugInformation"].GetBool(); }
        if (debug.HasMember("PrintModuleTimeout") && debug["PrintModuleTimeout"].IsBool()) { Options::Debug::PRINT_MODULE_TIMEOUT = debug["PrintModuleTimeout"].GetBool(); }
        if (debug.HasMember("ErrorLogFilename") && debug["ErrorLogFilename"].IsString()) { Options::Debug::ErrorLogFilename = debug["ErrorLogFilename"].GetString(); }
    }

    // Parse the Module Options
    if (doc.HasMember("Module Options") && doc["Module Options"].IsObject())
    {
        const rapidjson::Value & module = doc["Module Options"];
        if (module.HasMember("UseGameCommander") && module["UseGameCommander"].IsBool()) { Options::Modules::USING_GAMECOMMANDER = module["UseGameCommander"].GetBool(); }
        if (module.HasMember("UseScoutManager") && module["UseScoutManager"].IsBool()) { Options::Modules::USING_SCOUTMANAGER = module["UseScoutManager"].GetBool(); }
        if (module.HasMember("UseCombatCommander") && module["UseCombatCommander"].IsBool()) { Options::Modules::USING_COMBATCOMMANDER = module["UseCombatCommander"].GetBool(); }
        if (module.HasMember("UseBuildOrderSearch") && module["UseBuildOrderSearch"].IsBool()) { Options::Modules::USING_MACRO_SEARCH = module["UseBuildOrderSearch"].GetBool(); }
        if (module.HasMember("UseStrategyIO") && module["UseStrategyIO"].IsBool()) { Options::Modules::USING_STRATEGY_IO = module["UseStrategyIO"].GetBool(); }
        if (module.HasMember("UseUnitCommandManager") && module["UseUnitCommandManager"].IsBool()) { Options::Modules::USING_UNIT_COMMAND_MGR = module["UseUnitCommandManager"].GetBool(); }
    }

    // Parse the Tool Options
    if (doc.HasMember("Tool Options") && doc["Tool Options"].IsObject())
    {
        const rapidjson::Value & tool = doc["Tool Options"];
        if (tool.HasMember("MapGridSize") && tool["MapGridSize"].IsInt()) { Options::Tools::MAP_GRID_SIZE = tool["MapGridSize"].GetInt(); }
    }

    // Parse the Strategy Options
    if (doc.HasMember("Strategy Options") && doc["Strategy Options"].IsObject())
    {
        const rapidjson::Value & strategy = doc["Strategy Options"];
        if (strategy.HasMember("StrategyName") && strategy["StrategyName"].IsString()) { Options::Strategy::StrategyName = strategy["StrategyName"].GetString(); }

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
	if (Options::Modules::USING_GAMECOMMANDER)
	{
		StrategyManager::Instance().onEnd(isWinner);
        
		ProductionManager::Instance().onGameEnd();
	}	
}

void UAlbertaBotModule::onFrame()
{
	if (Options::Modules::USING_UNIT_COMMAND_MGR)
	{
		UnitCommandManager::Instance().update();
	}

	if (Options::Modules::USING_GAMECOMMANDER) 
	{ 
		gameCommander.update(); 
	}
}

void UAlbertaBotModule::onUnitDestroy(BWAPI::UnitInterface* unit)
{
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitDestroy(unit); }
}

void UAlbertaBotModule::onUnitMorph(BWAPI::UnitInterface* unit)
{
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitMorph(unit); }
}

void UAlbertaBotModule::onSendText(std::string text) 
{ 
	if (Options::Modules::USING_BUILD_ORDER_DEMO)
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
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitCreate(unit); }
}

void UAlbertaBotModule::onUnitComplete(BWAPI::UnitInterface* unit)
{
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitComplete(unit); }
}

void UAlbertaBotModule::onUnitShow(BWAPI::UnitInterface* unit)
{ 
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitShow(unit); }
}

void UAlbertaBotModule::onUnitHide(BWAPI::UnitInterface* unit)
{ 
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitHide(unit); }
}

void UAlbertaBotModule::onUnitRenegade(BWAPI::UnitInterface* unit)
{ 
	if (Options::Modules::USING_GAMECOMMANDER) { gameCommander.onUnitRenegade(unit); }
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