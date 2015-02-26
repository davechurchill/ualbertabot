#pragma once

#include "BOSS.h"
#include "GUI.h"
#include "GUITools.h"
#include "JSONTools.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "BOSSVisExperiment.h"
#include "CombatSearchExperiment.h"

namespace BOSS
{

class BOSSExperiments
{
    std::string                                         _configFileName;
    std::string                                         _jsonString;

    std::map< std::string, GameState >                  _stateMap;
    std::map< std::string, BuildOrder >                 _buildOrderMap;
    std::map< std::string, BuildOrderSearchGoal >       _buildOrderSearchGoalMap;

    std::vector< BOSSVisExperiment >                    _visExperiments;
    std::vector< CombatSearchExperiment >               _combatSearchExperiments;

    void parseParameters();
    void runBuildOrderVisualizationExperiment(const rapidjson::Value & val);
    void doCombatSearchExperiment(const rapidjson::Value & val, std::map< std::string, GameState > & _stateMap);

public:

    BOSSExperiments();

    void loadExperimentsFromFile(const std::string & configFileName);
    void loadExperimentsFromString(const std::string & jsonString);
    
    void onFrame();
    void parseExperiments();
    const GameState &                           getState(const std::string & key);
    const BuildOrder &                          getBuildOrder(const std::string & key);
    const BuildOrderSearchGoal &                getBuildOrderSearchGoalMap(const std::string & key);

    std::vector< BOSSVisExperiment > &          getVisExperiments();
    std::vector< CombatSearchExperiment > &     getCombatSearchExperiments();
};
}
