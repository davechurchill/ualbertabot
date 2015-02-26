#include "BOSSExperiments.h"

using namespace BOSS;

BOSSExperiments::BOSSExperiments()
{
}

void BOSSExperiments::loadExperimentsFromFile(const std::string & configFileName)
{
    _configFileName = configFileName;
    _jsonString = JSONTools::ReadJsonFile(configFileName);

    parseParameters();
    parseExperiments();
}

void BOSSExperiments::loadExperimentsFromString(const std::string & jsonString)
{
    _jsonString = jsonString;

    parseParameters();
    parseExperiments();
}

void BOSSExperiments::parseExperiments()
{
    rapidjson::Document document;
    JSONTools::ParseJSONString(document, _jsonString);

    BOSS_ASSERT(document.HasMember("Experiments"), "No 'Experiments' member found");

    const rapidjson::Value & experiments = document["Experiments"];
    for (rapidjson::Value::ConstMemberIterator itr = experiments.MemberBegin(); itr != experiments.MemberEnd(); ++itr)
    {
        const std::string &         name = itr->name.GetString();
        const rapidjson::Value &    val  = itr->value;
        
        //std::cout << "Found Experiment:   " << name << std::endl;
        BOSS_ASSERT(val.HasMember("type") && val["type"].IsString(), "Experiment has no 'type' string");

        if (val.HasMember("run") && val["run"].IsBool() && (val["run"].GetBool() == true))
        {            
            if (std::string(val["type"].GetString()).compare("BuildOrderVisualization") == 0)
            {
                _visExperiments.push_back(BOSSVisExperiment(val, _stateMap, _buildOrderMap));
            }

            if (std::string(val["type"].GetString()).compare("CombatSearch") == 0)
            {
                _combatSearchExperiments.push_back(CombatSearchExperiment(name, val, _stateMap, _buildOrderMap));
            }
        }
    }

    std::cout << "\n\n";
}

void BOSSExperiments::parseParameters()
{
    rapidjson::Document document;
    JSONTools::ParseJSONString(document, _jsonString);

    BOSS_ASSERT(document.HasMember("States"), "No 'States' member found");
    BOSS_ASSERT(document.HasMember("Build Orders"), "No 'Build Orders' member found");
//    BOSS_ASSERT(document.HasMember("Build Order Search Goals"), "No 'Build Order Goals' member found");

    const rapidjson::Value & states = document["States"];
    for (rapidjson::Value::ConstMemberIterator itr = states.MemberBegin(); itr != states.MemberEnd(); ++itr)
    {
        const std::string &         name = itr->name.GetString();
        const rapidjson::Value &    val  = itr->value;
               
        _stateMap[name] = JSONTools::GetGameState(val);
    }

    const rapidjson::Value & buildOrders = document["Build Orders"];
    for (rapidjson::Value::ConstMemberIterator itr = buildOrders.MemberBegin(); itr != buildOrders.MemberEnd(); ++itr)
    {
        const std::string &         name = itr->name.GetString();
        const rapidjson::Value &    val  = itr->value;
               
        _buildOrderMap[name] = JSONTools::GetBuildOrder(val);
    }

    if (document.HasMember("Build Order Search Goals"))
    {
        const rapidjson::Value & buildOrderGoals = document["Build Order Search Goals"];
        for (rapidjson::Value::ConstMemberIterator itr = buildOrderGoals.MemberBegin(); itr != buildOrderGoals.MemberEnd(); ++itr)
        {
            const std::string &         name = itr->name.GetString();
            const rapidjson::Value &    val  = itr->value;
               
            _buildOrderSearchGoalMap[name] = JSONTools::GetBuildOrderSearchGoal(val);
        }
    }
}

const GameState & BOSSExperiments::getState(const std::string & key)
{
    BOSS_ASSERT(_stateMap.find(key) != _stateMap.end(), "Couldn't find state: %s", key.c_str());

    return _stateMap[key];
}

const BuildOrder & BOSSExperiments::getBuildOrder(const std::string & key)
{
    BOSS_ASSERT(_buildOrderMap.find(key) != _buildOrderMap.end(), "Couldn't find build order: %s", key.c_str());

    return _buildOrderMap[key];
}

const BuildOrderSearchGoal & BOSSExperiments::getBuildOrderSearchGoalMap(const std::string & key)
{
    BOSS_ASSERT(_buildOrderSearchGoalMap.find(key) != _buildOrderSearchGoalMap.end(), "Couldn't find state: %s", key.c_str());

    return _buildOrderSearchGoalMap[key];
}

std::vector< BOSSVisExperiment > & BOSSExperiments::getVisExperiments()
{
    return _visExperiments;
}

std::vector< CombatSearchExperiment > & BOSSExperiments::getCombatSearchExperiments()
{
    return _combatSearchExperiments;
}