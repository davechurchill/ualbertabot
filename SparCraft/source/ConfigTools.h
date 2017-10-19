#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "rapidjson/document.h"

namespace SparCraft
{
namespace ConfigTools
{
    std::shared_ptr<Map> GetMapFromVariable(const std::string & mapVariable, const rapidjson::Value & root);
    GameState GetStateFromVariable(const std::string & stateVariable, const rapidjson::Value & root);
}
}
