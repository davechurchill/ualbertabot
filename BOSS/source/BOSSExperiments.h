#pragma once

#include "BOSS.h"
#include "GUI.h"
#include "GUITools.h"
#include "JSONTools.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "BOSSVisExperiment.h"

namespace BOSS
{

class BOSSExperiments
{
    std::string                                         _configFileName;
    std::string                                         _jsonString;

    std::map< std::string, GameState >                  _stateMap;
    std::map< std::string, std::vector<ActionType> >    _buildOrderMap;
    std::map< std::string, BuildOrderSearchGoal >       _buildOrderSearchGoalMap;

    std::vector< BOSSVisExperiment >                    _visExperiments;

    void parseParameters();
    void runBuildOrderVisualizationExperiment(const rapidjson::Value & val);

public:

    BOSSExperiments();

    void loadExperimentsFromFile(const std::string & configFileName);
    void loadExperimentsFromString(const std::string & jsonString);
    
    void onFrame();
    void parseExperiments();
    const GameState &                   getState(const std::string & key);
    const std::vector<ActionType> &     getBuildOrder(const std::string & key);
    const BuildOrderSearchGoal &   getBuildOrderSearchGoalMap(const std::string & key);

    std::vector< BOSSVisExperiment > &  getVisExperiments();
};
}
