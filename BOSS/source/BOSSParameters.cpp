#include "BOSSParameters.h"

using namespace BOSS;

BOSSParameters::BOSSParameters()
{

}

BOSSParameters & BOSSParameters::Instance()
{
    static BOSSParameters params;
    return params;
}

void BOSSParameters::ParseParameters(const std::string & configFile)
{
    _configFile = configFile;

    rapidjson::Document document;
    JSONTools::ParseJSONFile(document, _configFile);

    BOSS_ASSERT(document.HasMember("States"), "No 'States' member found");
    BOSS_ASSERT(document.HasMember("Build Orders"), "No 'Build Orders' member found");
//    BOSS_ASSERT(document.HasMember("Build Order Search Goals"), "No 'Build Order Goals' member found");

    // Parse all the States
    const rapidjson::Value & states = document["States"];
    for (rapidjson::Value::ConstMemberIterator itr = states.MemberBegin(); itr != states.MemberEnd(); ++itr)
    {
        const std::string &         name = itr->name.GetString();
        const rapidjson::Value &    val  = itr->value;
               
        _stateMap[name] = JSONTools::GetGameState(val);
    }

    // Parse all the Build Orders
    const rapidjson::Value & buildOrders = document["Build Orders"];
    for (rapidjson::Value::ConstMemberIterator itr = buildOrders.MemberBegin(); itr != buildOrders.MemberEnd(); ++itr)
    {
        const std::string &         name = itr->name.GetString();
        const rapidjson::Value &    val  = itr->value;
               
        _buildOrderMap[name] = JSONTools::GetBuildOrder(val);
    }

    // Parse all the Build Order Goals
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

const GameState & BOSSParameters::GetState(const std::string & key)
{
    BOSS_ASSERT(_stateMap.find(key) != _stateMap.end(), "Couldn't find state: %s", key.c_str());

    return _stateMap[key];
}

const BuildOrder & BOSSParameters::GetBuildOrder(const std::string & key)
{
    BOSS_ASSERT(_buildOrderMap.find(key) != _buildOrderMap.end(), "Couldn't find build order: %s", key.c_str());

    return _buildOrderMap[key];
}

const BuildOrderSearchGoal & BOSSParameters::GetBuildOrderSearchGoalMap(const std::string & key)
{
    BOSS_ASSERT(_buildOrderSearchGoalMap.find(key) != _buildOrderSearchGoalMap.end(), "Couldn't find state: %s", key.c_str());

    return _buildOrderSearchGoalMap[key];
}