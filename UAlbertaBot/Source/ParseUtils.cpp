#include "ParseUtils.h"
#include "rapidjson\document.h"
#include "JSONTools.h"
#include "BuildOrder.h"
#include "StrategyManager.h"

using namespace UAlbertaBot;

inline bool file_exists(const std::string& name) {
	if (FILE *file = fopen(name.c_str(), "r")) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}

std::pair<bool, std::string> findPlayerSpecificStrategy(const rapidjson::Value & strategy, const char * ourRace, const BWAPI::Player& player)
{
	const auto enemyName = player->getName().c_str();
	const auto & specific = strategy["EnemySpecificStrategy"];

	// check to see if our current enemy name is listed anywhere in the specific strategies
	if (specific.HasMember(enemyName) && specific[enemyName].IsObject())
	{
		const auto & enemyStrategies = specific[enemyName];

		// if that enemy has a strategy listed for our current race, use it
		if (enemyStrategies.HasMember(ourRace) && enemyStrategies[ourRace].IsString())
		{
			return std::make_pair(true, enemyStrategies[ourRace].GetString());
		}
	}

	return std::make_pair(false, std::string());
}

std::string ParseUtils::FindConfigurationLocation(const std::string & filename)
{
	auto bwapiAILocation = "bwapi-data/AI/" + filename;
	if (file_exists(bwapiAILocation)) {
		return bwapiAILocation;
	}

	return filename;
}

BWAPI::Race GetRace(const std::string & raceName)
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

void UAlbertaBot::ParseUtils::ParseConfigFile(
	const std::string & filename,
	BotConfiguration& config,
	bool& configFileFound,
	bool& configFileParsed)
{
	configFileFound = true;
	configFileParsed = true;
    std::string configContent = ReadFile(filename);

    if (configContent.length() == 0)
    {
        configFileFound = false;
        return;
    }

	rapidjson::Document doc;
	doc.Parse(configContent.c_str());
	bool parsingFailed = doc.HasParseError();
    if (parsingFailed)
    {
        configFileParsed = false;
        return;
    }

    // Parse the Bot Info
    if (doc.HasMember("Bot Info") && doc["Bot Info"].IsObject())
    {
        const rapidjson::Value & info = doc["Bot Info"];
		auto& botInfo = config.BotInfo;
        JSONTools::ReadString("BotName", info, botInfo.BotName);
        JSONTools::ReadString("Authors", info, botInfo.Authors);
        JSONTools::ReadBool("PrintInfoOnStart", info, botInfo.PrintInfoOnStart);
        JSONTools::ReadString("BotMode", info, botInfo.BotMode);
    }

    // Parse the BWAPI Options
    if (doc.HasMember("BWAPI") && doc["BWAPI"].IsObject())
    {
        const rapidjson::Value & bwapi = doc["BWAPI"];
		auto& bwapiOptions = config.BWAPIOptions;
        JSONTools::ReadInt("SetLocalSpeed", bwapi, bwapiOptions.SetLocalSpeed);
        JSONTools::ReadInt("SetFrameSkip", bwapi, bwapiOptions.SetFrameSkip);
        JSONTools::ReadBool("UserInput", bwapi, bwapiOptions.EnableUserInput);
        JSONTools::ReadBool("CompleteMapInformation", bwapi, bwapiOptions.EnableCompleteMapInformation);
    }

    // Parse the Micro Options
    if (doc.HasMember("Micro") && doc["Micro"].IsObject())
    {
        const rapidjson::Value & micro = doc["Micro"];
		auto& microOptions = config.Micro;
        JSONTools::ReadBool("UseSparcraftSimulation", micro, microOptions.UseSparcraftSimulation);
        JSONTools::ReadBool("KiteWithRangedUnits", micro, microOptions.KiteWithRangedUnits);
        JSONTools::ReadBool("WorkersDefendRush", micro, microOptions.WorkersDefendRush);
        JSONTools::ReadInt("RetreatMeleeUnitShields", micro, microOptions.RetreatMeleeUnitShields);
        JSONTools::ReadInt("RetreatMeleeUnitHP", micro, microOptions.RetreatMeleeUnitHP);
        JSONTools::ReadInt("InCombatRadius", micro, microOptions.CombatRadius);
        JSONTools::ReadInt("RegroupRadius", micro, microOptions.CombatRegroupRadius);
        JSONTools::ReadInt("UnitNearEnemyRadius", micro, microOptions.UnitNearEnemyRadius);

        if (micro.HasMember("KiteLongerRangedUnits") && micro["KiteLongerRangedUnits"].IsArray())
        {
            const rapidjson::Value & kite = micro["KiteLongerRangedUnits"];

            for (size_t i(0); i < kite.Size(); ++i)
            {
                if (kite[i].IsString())
                {
                    MetaType type(kite[i].GetString());
					microOptions.KiteLongerRangedUnits.insert(type.getUnitType());
                }
            }
        }
    }

    // Parse the Macro Options
    if (doc.HasMember("Macro") && doc["Macro"].IsObject())
    {
        const rapidjson::Value & macro = doc["Macro"];
		auto& macroOptions = config.Macro;
        JSONTools::ReadInt("BOSSFrameLimit", macro, macroOptions.BOSSFrameLimit);
        JSONTools::ReadInt("BuildingSpacing", macro, macroOptions.BuildingSpacing);
        JSONTools::ReadInt("PylongSpacing", macro, macroOptions.PylonSpacing);
        JSONTools::ReadInt("WorkersPerRefinery", macro, macroOptions.WorkersPerRefinery);
	}

	// Parse the Debug Options
	if (doc.HasMember("Log") && doc["Log"].IsObject())
	{
		const rapidjson::Value & log = doc["Log"];
		auto& logOptions = config.Log;
		JSONTools::ReadString("ErrorLogFilename", log, logOptions.ErrorLogFilename);
		JSONTools::ReadBool("LogAssertToErrorFile", log, logOptions.LogAssertToErrorFile);
    }

    // Parse the Debug Options
    if (doc.HasMember("Debug") && doc["Debug"].IsObject())
    {
        const rapidjson::Value & debug = doc["Debug"];
		auto& debugOptions = config.Debug;
        JSONTools::ReadBool("DrawGameInfo",             debug, debugOptions.DrawGameInfo);
        JSONTools::ReadBool("DrawBuildOrderSearchInfo", debug, debugOptions.DrawBuildOrderSearchInfo);
        JSONTools::ReadBool("DrawUnitHealthBars",       debug, debugOptions.DrawUnitHealthBars);
        JSONTools::ReadBool("DrawResourceInfo",         debug, debugOptions.DrawResourceInfo);
        JSONTools::ReadBool("DrawWorkerInfo",           debug, debugOptions.DrawWorkerInfo);
        JSONTools::ReadBool("DrawProductionInfo",       debug, debugOptions.DrawProductionInfo);
        JSONTools::ReadBool("DrawScoutInfo",            debug, debugOptions.DrawScoutInfo);
        JSONTools::ReadBool("DrawSquadInfo",            debug, debugOptions.DrawSquadInfo);
        JSONTools::ReadBool("DrawCombatSimInfo",        debug, debugOptions.DrawCombatSimulationInfo);
        JSONTools::ReadBool("DrawBuildingInfo",         debug, debugOptions.DrawBuildingInfo);
        JSONTools::ReadBool("DrawModuleTimers",         debug, debugOptions.DrawModuleTimers);
        JSONTools::ReadBool("DrawMouseCursorInfo",      debug, debugOptions.DrawMouseCursorInfo);
        JSONTools::ReadBool("DrawEnemyUnitInfo",        debug, debugOptions.DrawEnemyUnitInfo);
        JSONTools::ReadBool("DrawBWTAInfo",             debug, debugOptions.DrawBWTAInfo);
        JSONTools::ReadBool("DrawLastSeenTileInfo",     debug, debugOptions.DrawLastSeenTileInfo);
        JSONTools::ReadBool("DrawUnitTargetInfo",       debug, debugOptions.DrawUnitTargetInfo);
        JSONTools::ReadBool("DrawReservedBuildingTiles",debug, debugOptions.DrawReservedBuildingTiles);
        JSONTools::ReadBool("DrawBOSSStateInfo",        debug, debugOptions.DrawBOSSStateInfo); 
        JSONTools::ReadBool("PrintModuleTimeout",       debug, debugOptions.PrintModuleTimeout);
    }

    // Parse the Module Options
    if (doc.HasMember("Modules") && doc["Modules"].IsObject())
    {
        const rapidjson::Value & module = doc["Modules"];
		auto& modulesOptions = config.Modules;

        JSONTools::ReadBool("UseBuildOrderSearch", module, modulesOptions.UsingBuildOrderSearch);
        JSONTools::ReadBool("UseAutoObserver", module, modulesOptions.UsingAutoObserver);
    }

    // Parse the Tool Options
    if (doc.HasMember("Tools") && doc["Tools"].IsObject())
    {
        const rapidjson::Value & tool = doc["Tools"];
		auto& toolsOptions = config.Tools;

        JSONTools::ReadInt("MapGridSize", tool, toolsOptions.MAP_GRID_SIZE);
    }

    // Parse the SparCraft Options
    if (doc.HasMember("SparCraft") && doc["SparCraft"].IsObject())
    {
        const rapidjson::Value & sc = doc["SparCraft"];
		auto& sparcraftOptions = config.SparCraft;

        JSONTools::ReadString("SparCraftConfigFile", sc, sparcraftOptions.SparCraftConfigFile);
        JSONTools::ReadString("CombatSimPlayerName", sc, sparcraftOptions.CombatSimPlayerName);
    }

    if (doc.HasMember("Arena") && doc["Arena"].IsObject())
    {
        const rapidjson::Value & arena = doc["Arena"];
		auto& arenaOptions = config.Arena;
        JSONTools::ReadString("ArenaPlayerName", arena, arenaOptions.ArenaPlayerName);
        JSONTools::ReadInt("ArenaBattles", arena, arenaOptions.ArenaBattles);
        JSONTools::ReadInt("ArenaOutputResults", arena, arenaOptions.ArenaOutputResults);
    }
}

void ParseUtils::ParseStrategy(
	const std::string & filename,
	BotConfiguration& config, 
	shared_ptr<StrategyManager> strategyManager)
{
    BWAPI::Race race = BWAPI::Broodwar->self()->getRace();
    const char * ourRace = race.getName().c_str();
    std::string configContent = ReadFile(filename);
    rapidjson::Document doc;
    bool parsingFailed = doc.Parse(configContent.c_str()).HasParseError();
    if (parsingFailed)
    {
        std::cerr << "ParseStrategy could not find file: " << filename << ", shutting down.\n";
        return;
    }

    // Parse the Strategy Options
    if (doc.HasMember("Strategy") && doc["Strategy"].IsObject())
    {
        const rapidjson::Value & strategy = doc["Strategy"];
		auto& strategyOptions = config.Strategy;

        // read in the various strategic elements
        JSONTools::ReadBool("ScoutHarassEnemy", strategy, strategyOptions.ScoutHarassEnemy);

		JSONTools::ReadBool("UseStrategyIO", strategy, strategyOptions.UsingStrategyIO);
		JSONTools::ReadString("ReadDirectory", strategy, strategyOptions.ReadDir);
		JSONTools::ReadString("WriteDirectory", strategy, strategyOptions.WriteDir);

        // if we have set a strategy for the current race, use it
        if (strategy.HasMember(race.c_str()) && strategy[race.c_str()].IsString())
        {
			strategyOptions.StrategyName = strategy[race.c_str()].GetString();
        }

        // check if we are using an enemy specific strategy
        JSONTools::ReadBool("UseEnemySpecificStrategy", strategy, strategyOptions.UseEnemySpecificStrategy);
        if (strategyOptions.UseEnemySpecificStrategy && strategy.HasMember("EnemySpecificStrategy") && strategy["EnemySpecificStrategy"].IsObject())
        {
			for (const auto& enemy : BWAPI::Broodwar->enemies())
			{
				auto searchResult = findPlayerSpecificStrategy(strategy, ourRace, enemy);
				if (searchResult.first)
				{
					strategyOptions.StrategyName = searchResult.second;
					strategyOptions.FoundEnemySpecificStrategy = true;
					break;
				}
			}
        }

        // Parse all the Strategies
        if (strategy.HasMember("Strategies") && strategy["Strategies"].IsObject())
        {
            const rapidjson::Value & strategies = strategy["Strategies"];
            for (auto itr = strategies.MemberBegin(); itr != strategies.MemberEnd(); ++itr)
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

				Strategy strategyInstance(name, strategyRace, buildOrder);
                strategyManager->addStrategy(name, strategyInstance);
            }
        }
    }
}

std::string ParseUtils::ReadFile(const std::string & filename)
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