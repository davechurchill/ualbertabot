#pragma once

#include "BOSS.h"
#include "Common.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

namespace BOSS
{
namespace JSONTools
{
    std::string ReadJsonFile(const std::string & filename);
    void ParseJSONString(rapidjson::Document & document, const std::string & json);
    void ParseJSONFile(rapidjson::Document & document, const std::string & filename);

    GameState GetGameState(const std::string & jsonString);
    GameState GetGameState(const rapidjson::Value & stateVal);

    std::vector<ActionType> GetBuildOrder(const std::string & jsonString);
    std::vector<ActionType> GetBuildOrder(const rapidjson::Value & stateVal);

    DFBB_BuildOrderSearchGoal GetBuildOrderSearchGoal(const std::string & jsonString);
    DFBB_BuildOrderSearchGoal GetBuildOrderSearchGoal(const rapidjson::Value & stateVal);
}
}
