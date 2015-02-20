#include "CombatSearchExperiment.h"

using namespace BOSS;

CombatSearchExperiment::CombatSearchExperiment()
    : _race(Races::None)
{

}

CombatSearchExperiment::CombatSearchExperiment(const std::string & name, const rapidjson::Value & val, std::map< std::string, GameState > & stateMap, std::map< std::string, BuildOrder > & buildOrderMap)
    : _race(Races::None)
    , _name(name)
{
    BOSS_ASSERT(val.HasMember("searchTypes") && val["searchTypes"].IsArray(), "CombatSearchExperiment must have a 'searchTypes' array");
    for (size_t i(0); i < val["searchTypes"].Size(); ++i)
    {
        BOSS_ASSERT(val["searchTypes"][i].IsString(), "searchTypes element is not a string");
        
        _searchTypes.push_back(val["searchTypes"][i].GetString());
    }

    BOSS_ASSERT(val.HasMember("race") && val["race"].IsString(), "CombatSearchExperiment must have a 'race' string");
    _race = Races::GetRaceID(val["race"].GetString());

    BOSS_ASSERT(val.HasMember("state") && val["state"].IsString(), "CombatSearchExperiment must have a 'state' string");
    std::string stateName = val["state"].GetString();
    BOSS_ASSERT(stateMap.find(stateName) != stateMap.end(), "State not found: %s", stateName.c_str());
    BOSS_ASSERT(stateMap[stateName].getRace() == _race, "State race does not match experiment declared race");
    _params.setInitialState(stateMap[stateName]);

    BOSS_ASSERT(val.HasMember("frameTimeLimit") && val["frameTimeLimit"].IsInt(), "CombatSearchExperiment must have a 'frameTimeLimit' int");
    _params.setFrameTimeLimit(val["frameTimeLimit"].GetInt());

    BOSS_ASSERT(val.HasMember("searchTimeLimitMS") && val["searchTimeLimitMS"].IsInt(), "CombatSearchExperiment must have a 'searchTimeLimitMS' int");
    _params.setSearchTimeLimit(val["searchTimeLimitMS"].GetInt());

    if (val.HasMember("maxActions"))
    {
        const rapidjson::Value & maxActions = val["maxActions"];
        BOSS_ASSERT(maxActions.IsArray(), "maxActions is not an array");

        for (size_t i(0); i < maxActions.Size(); ++i)
        {
            BOSS_ASSERT(maxActions[i].IsArray(), "maxActions element must be array of size 2");

            BOSS_ASSERT(maxActions[i].Size() == 2 && maxActions[i][0u].IsString() && maxActions[i][1u].IsInt(), "maxActions element must be [\"Action\", Count]");

            BOSS_ASSERT(ActionTypes::TypeExists(maxActions[i][0u].GetString()), "Action Type doesn't exist: %s", maxActions[i][0u].GetString());

            _params.setMaxActions(ActionTypes::GetActionType(maxActions[i][0u].GetString()), maxActions[i][1].GetInt());
        }
    }

    if (val.HasMember("relevantActions"))
    {
        const rapidjson::Value & relevantActions = val["relevantActions"];
        BOSS_ASSERT(relevantActions.IsArray(), "relevantActions is not an array");

        ActionSet relevantActionSet;

        for (size_t i(0); i < relevantActions.Size(); ++i)
        {
            BOSS_ASSERT(relevantActions[i].IsString(), "relvantActions element must be action type string");
            BOSS_ASSERT(ActionTypes::TypeExists(relevantActions[i].GetString()), "Action Type doesn't exist: %s", relevantActions[i].GetString());
            
            relevantActionSet.add(ActionTypes::GetActionType(relevantActions[i].GetString()));
        }

        _params.setRelevantActions(relevantActionSet);
    }

    if (val.HasMember("alwaysMakeWorkers"))
    {
        BOSS_ASSERT(val["alwaysMakeWorkers"].IsBool(), "alwaysMakeWorkers should be a bool");

        _params.setAlwaysMakeWorkers(val["alwaysMakeWorkers"].GetBool());
    }

    if (val.HasMember("openingBuildOrder"))
    {
        BOSS_ASSERT(val["openingBuildOrder"].IsString(), "openingBuildOrder should be a string");

        std::string openingBuildOrderString = val["openingBuildOrder"].GetString();
        BOSS_ASSERT(buildOrderMap.find(openingBuildOrderString) != buildOrderMap.end(), "Opening build order not found: %s", openingBuildOrderString.c_str());
        _params.setOpeningBuildOrder(buildOrderMap[openingBuildOrderString]);
    }

    if (val.HasMember("bestResponseParams"))
    {
        const rapidjson::Value & brVal = val["bestResponseParams"];

        BOSS_ASSERT(brVal.IsObject(), "bestResponseParams not an object");
        BOSS_ASSERT(brVal.HasMember("enemyState"), "bestResponseParams must have 'enemyState' string");
        BOSS_ASSERT(brVal.HasMember("enemyBuildOrder"), "bestResponseParams must have 'enemyBuildOrder' string");

        BOSS_ASSERT(brVal.HasMember("enemyState") && brVal["enemyState"].IsString(), "bestResponseParams must have a 'enemyState' string");
        std::string enemyStateName = brVal["enemyState"].GetString();
        BOSS_ASSERT(stateMap.find(enemyStateName) != stateMap.end(), "Enemy State not found: %s", enemyStateName.c_str());
        BOSS_ASSERT(stateMap[enemyStateName].getRace() == _race, "State race does not match experiment declared race");
        _params.setEnemyInitialState(stateMap[enemyStateName]);

        BOSS_ASSERT(brVal.HasMember("enemyBuildOrder") && brVal["enemyBuildOrder"].IsString(), "bestResponseParams must have a 'enemyBuildOrder' string");
        std::string enemyBuildOrderString = brVal["enemyBuildOrder"].GetString();
        BOSS_ASSERT(buildOrderMap.find(enemyBuildOrderString) != buildOrderMap.end(), "Enemy build order not found: %s", enemyStateName.c_str());
        _params.setEnemyBuildOrder(buildOrderMap[enemyBuildOrderString]);
    }
}

void CombatSearchExperiment::run()
{
    static std::string stars = "************************************************";
    for (size_t i(0); i < _searchTypes.size(); ++i)
    {
        std::shared_ptr<CombatSearch> combatSearch;
        std::string resultsFile = "gnuplot/" + _name;

        std::cout << "\n" << stars << "\n* Running Experiment: " << _name << " [" << _searchTypes[i] << "]\n" << stars << "\n";

        if (_searchTypes[i].compare("Integral") == 0)
        {
            combatSearch = std::shared_ptr<CombatSearch>(new CombatSearch_Integral(_params));
            resultsFile += "_Integral"; 
        }
        else if (_searchTypes[i].compare("Bucket") == 0)
        {
            combatSearch = std::shared_ptr<CombatSearch>(new CombatSearch_Bucket(_params));
            resultsFile += "_Bucket"; 
        }
        else if (_searchTypes[i].compare("BestResponse") == 0)
        {
            combatSearch = std::shared_ptr<CombatSearch>(new CombatSearch_BestResponse(_params));
            resultsFile += "_BestResponse"; 
        }
        else
        {
            BOSS_ASSERT(false, "CombatSearch type not found: %s", _searchTypes[i].c_str());
        }

        combatSearch->search();
        combatSearch->printResults();
        combatSearch->writeResultsFile(resultsFile);
        const CombatSearchResults & results = combatSearch->getResults();
        std::cout << "\nSearched " << results.nodesExpanded << " nodes in " << results.timeElapsed << "ms @ " << (1000.0*results.nodesExpanded/results.timeElapsed) << " nodes/sec\n\n";
    }
}