#pragma once

#include "BOSS.h"
#include "JSONTools.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

namespace BOSS
{

class BOSSExperiments
{
    std::string                                         _configFileName;

    std::map< std::string, GameState >                  _stateMap;
    std::map< std::string, std::vector<ActionType> >    _buildOrderMap;
    std::map< std::string, DFBB_BuildOrderSearchGoal >  _buildOrderSearchGoalMap;

    void parseParameters();
    void runBuildOrderVisualizationExperiment(const rapidjson::Value & val);

public:

    BOSSExperiments(const std::string & configFileName);
    
    void runExperiments();
    const GameState &                   getState(const std::string & key);
    const std::vector<ActionType> &     getBuildOrder(const std::string & key);
    const DFBB_BuildOrderSearchGoal &   getBuildOrderSearchGoalMap(const std::string & key);
};
}
