#pragma once

#include "BOSS.h"
#include "Common.h"
#include "BuildOrder.h"
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

    BuildOrder GetBuildOrder(const std::string & jsonString);
    BuildOrder GetBuildOrder(const rapidjson::Value & stateVal);

    BuildOrderSearchGoal GetBuildOrderSearchGoal(const std::string & jsonString);
    BuildOrderSearchGoal GetBuildOrderSearchGoal(const rapidjson::Value & stateVal);

    std::string GetBuildOrderString(const std::vector<ActionType> & buildOrder);
}
}
