#include "BOSSExperiments.h"

using namespace BOSS;

BOSSExperiments::BOSSExperiments(const std::string & configFileName)
    : _configFileName(configFileName)
{
    parseParameters();
}

void BOSSExperiments::runExperiments()
{
    rapidjson::Document document;
    JSONTools::ParseJSONFile(document, _configFileName);

    BOSS_ASSERT(document.HasMember("Experiments"), "No 'Experiments' member found");

    const rapidjson::Value & experiments = document["Experiments"];
    for (rapidjson::Value::ConstMemberIterator itr = experiments.MemberBegin(); itr != experiments.MemberEnd(); ++itr)
    {
        const std::string &         name = itr->name.GetString();
        const rapidjson::Value &    val  = itr->value;
        
        std::cout << name << std::endl;
        BOSS_ASSERT(val.HasMember("type") && val["type"].IsString(), "Experiment has no 'type' string");

        if (val.HasMember("run") && val["run"].IsBool() && (val["run"].GetBool() == true))
        {
            std::cout << "Running Experiment: " << name << std::endl;
            
            if (std::string(val["type"].GetString()).compare("BuildOrderVisualization") == 0)
            {
                runBuildOrderVisualizationExperiment(val);
            }
        }
    }
}

void BOSSExperiments::runBuildOrderVisualizationExperiment(const rapidjson::Value & val)
{
    BOSS_ASSERT(val.HasMember("scenarios") && val["scenarios"].IsArray(), "BOVis has no scenarios");

    std::vector< GameState >                        states;
    std::vector< std::vector<ActionType> >          buildOrders;
    std::vector< std::vector<FrameCountType> >      startTimes(val["scenarios"].Size(), std::vector<FrameCountType>());
    std::vector< std::vector<FrameCountType> >      finishTimes(val["scenarios"].Size(), std::vector<FrameCountType>());;
    std::vector< size_t >                           nextActionIndexes(val["scenarios"].Size(), 0);

    double frameDelayMS = 0;
    if (val.HasMember("fps") && val["fps"].IsInt())
    {
        frameDelayMS = 1000.0 / val["fps"].GetInt();
    }

    const rapidjson::Value & scenarios = val["scenarios"];
    for (size_t i(0); i < scenarios.Size(); ++i)
    {
        const rapidjson::Value & scenario = scenarios[i];

        BOSS_ASSERT(scenario.HasMember("state") && scenario["state"].IsString(), "Scenario has no 'state' string");
        BOSS_ASSERT(scenario.HasMember("buildOrder") && scenario["buildOrder"].IsString(), "Scenario has no 'buildOrder' string");

        states.push_back(getState(scenario["state"].GetString()));
        buildOrders.push_back(getBuildOrder(scenario["buildOrder"].GetString()));
    }

    BOSS::GUI::Instance().OnStart();
    
    while (true)
    {
        Timer t;
        t.start();

        for (size_t s(0); s < states.size(); ++s)
        {
            bool didAction = false;

            if (nextActionIndexes[s] < buildOrders[s].size())
            {
                FrameCountType nextActionFrame = states[s].whenCanPerform(buildOrders[s][nextActionIndexes[s]]);

                if (nextActionFrame == states[s].getCurrentFrame())
                {
                    ActionType type = buildOrders[s][nextActionIndexes[s]];
                    FrameCountType finish = states[s].getCurrentFrame() + buildOrders[s][nextActionIndexes[s]].buildTime();
                    if (type.isBuilding() && !type.isAddon() && !type.isMorphed())
                    {
                        finish += Constants::BUILDING_PLACEMENT;
                    }

                    startTimes[s].push_back(states[s].getCurrentFrame());
                    finishTimes[s].push_back(finish);

                    states[s].doAction(buildOrders[s][nextActionIndexes[s]]);

                    didAction = true;
                    //std::cout << states[s].getCurrentFrame() << " Action Performed: " << buildOrder[nextActionIndex].getName() << std::endl;
                    nextActionIndexes[s]++;
                }
            }
        
            if (!didAction)
            {
                states[s].fastForward(states[s].getCurrentFrame() + 1);
            }

            BOSS::GUI::Instance().AddState(states[s]);
            BOSS::GUI::Instance().AddActionTimes(startTimes[s], finishTimes[s]);
            BOSS::GUI::Instance().AddBuildOrder(buildOrders[s], nextActionIndexes[s]);
        }
        
        BOSS::GUI::Instance().OnFrame();
        while (t.getElapsedTimeInMilliSec() < frameDelayMS) {}

        bool anyLess = false;
        for (size_t i(0); i < states.size(); ++i)
        {
            if (nextActionIndexes[i] < buildOrders[i].size() || states[i].getCurrentFrame() < states[i].getLastActionFinishTime())
            {
                anyLess = true;
                break;
            }
        }

        if (!anyLess)
        {
            break;
        }
    }
}

void BOSSExperiments::parseParameters()
{
    rapidjson::Document document;
    JSONTools::ParseJSONFile(document, _configFileName);

    BOSS_ASSERT(document.HasMember("States"), "No 'States' member found");
    BOSS_ASSERT(document.HasMember("Build Orders"), "No 'Build Orders' member found");
    BOSS_ASSERT(document.HasMember("Build Order Search Goals"), "No 'Build Order Goals' member found");

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

    const rapidjson::Value & buildOrderGoals = document["Build Order Search Goals"];
    for (rapidjson::Value::ConstMemberIterator itr = buildOrderGoals.MemberBegin(); itr != buildOrderGoals.MemberEnd(); ++itr)
    {
        const std::string &         name = itr->name.GetString();
        const rapidjson::Value &    val  = itr->value;
               
        _buildOrderSearchGoalMap[name] = JSONTools::GetBuildOrderSearchGoal(val);
    }
}

const GameState & BOSSExperiments::getState(const std::string & key)
{
    BOSS_ASSERT(_stateMap.find(key) != _stateMap.end(), "Couldn't find state: %s", key.c_str());

    return _stateMap[key];
}

const std::vector<ActionType> & BOSSExperiments::getBuildOrder(const std::string & key)
{
    BOSS_ASSERT(_buildOrderMap.find(key) != _buildOrderMap.end(), "Couldn't find build order: %s", key.c_str());

    return _buildOrderMap[key];
}

const DFBB_BuildOrderSearchGoal & BOSSExperiments::getBuildOrderSearchGoalMap(const std::string & key)
{
    BOSS_ASSERT(_buildOrderSearchGoalMap.find(key) != _buildOrderSearchGoalMap.end(), "Couldn't find state: %s", key.c_str());

    return _buildOrderSearchGoalMap[key];
}
