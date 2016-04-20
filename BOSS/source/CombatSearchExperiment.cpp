#include "CombatSearchExperiment.h"
#include "BOSSParameters.h"

using namespace BOSS;

CombatSearchExperiment::CombatSearchExperiment()
    : _race(Races::None)
{

}

CombatSearchExperiment::CombatSearchExperiment(const std::string & name, const rapidjson::Value & val)
    : _race(Races::None)
    , _name(name)
{
    BOSS_ASSERT(val.HasMember("SearchTypes") && val["SearchTypes"].IsArray(), "CombatSearchExperiment must have a 'SearchTypes' array");
    for (size_t i(0); i < val["SearchTypes"].Size(); ++i)
    {
        BOSS_ASSERT(val["SearchTypes"][i].IsString(), "searchTypes element is not a string");
        
        _searchTypes.push_back(val["SearchTypes"][i].GetString());
    }

    BOSS_ASSERT(val.HasMember("Race") && val["Race"].IsString(), "CombatSearchExperiment must have a 'Race' string");
    _race = Races::GetRaceID(val["Race"].GetString());

    BOSS_ASSERT(val.HasMember("State") && val["State"].IsString(), "CombatSearchExperiment must have a 'State' string");
    _params.setInitialState(BOSSParameters::Instance().GetState(val["State"].GetString()));

    BOSS_ASSERT(val.HasMember("FrameTimeLimit") && val["FrameTimeLimit"].IsInt(), "CombatSearchExperiment must have a 'FrameTimeLimit' int");
    _params.setFrameTimeLimit(val["FrameTimeLimit"].GetInt());

    BOSS_ASSERT(val.HasMember("SearchTimeLimitMS") && val["SearchTimeLimitMS"].IsInt(), "CombatSearchExperiment must have a 'SearchTimeLimitMS' int");
    _params.setSearchTimeLimit(val["SearchTimeLimitMS"].GetInt());

    if (val.HasMember("MaxActions"))
    {
        const rapidjson::Value & maxActions = val["MaxActions"];
        BOSS_ASSERT(maxActions.IsArray(), "MaxActions is not an array");

        for (size_t i(0); i < maxActions.Size(); ++i)
        {
            BOSS_ASSERT(maxActions[i].IsArray(), "MaxActions element must be array of size 2");

            BOSS_ASSERT(maxActions[i].Size() == 2 && maxActions[i][0u].IsString() && maxActions[i][1u].IsInt(), "MaxActions element must be [\"Action\", Count]");

            BOSS_ASSERT(ActionTypes::TypeExists(maxActions[i][0u].GetString()), "Action Type doesn't exist: %s", maxActions[i][0u].GetString());

            _params.setMaxActions(ActionTypes::GetActionType(maxActions[i][0u].GetString()), maxActions[i][1].GetInt());
        }
    }

    if (val.HasMember("RelevantActions"))
    {
        const rapidjson::Value & relevantActions = val["RelevantActions"];
        BOSS_ASSERT(relevantActions.IsArray(), "RelevantActions is not an array");

        ActionSet relevantActionSet;

        for (size_t i(0); i < relevantActions.Size(); ++i)
        {
            BOSS_ASSERT(relevantActions[i].IsString(), "RelvantActions element must be action type string");
            BOSS_ASSERT(ActionTypes::TypeExists(relevantActions[i].GetString()), "Action Type doesn't exist: %s", relevantActions[i].GetString());
            
            relevantActionSet.add(ActionTypes::GetActionType(relevantActions[i].GetString()));
        }

        _params.setRelevantActions(relevantActionSet);
    }

    if (val.HasMember("AlwaysMakeWorkers"))
    {
        BOSS_ASSERT(val["AlwaysMakeWorkers"].IsBool(), "AlwaysMakeWorkers should be a bool");

        _params.setAlwaysMakeWorkers(val["AlwaysMakeWorkers"].GetBool());
    }

    if (val.HasMember("OpeningBuildOrder"))
    {
        BOSS_ASSERT(val["OpeningBuildOrder"].IsString(), "OpeningBuildOrder should be a string");
        _params.setOpeningBuildOrder(BOSSParameters::Instance().GetBuildOrder(val["OpeningBuildOrder"].GetString()));
    }

    if (val.HasMember("BestResponseParams"))
    {
        const rapidjson::Value & brVal = val["BestResponseParams"];

        BOSS_ASSERT(brVal.IsObject(), "BestResponseParams not an object");
        BOSS_ASSERT(brVal.HasMember("EnemyState"), "bestResponseParams must have 'enemyState' string");
        BOSS_ASSERT(brVal.HasMember("EnemyBuildOrder"), "bestResponseParams must have 'enemyBuildOrder' string");

        BOSS_ASSERT(brVal.HasMember("EnemyState") && brVal["EnemyState"].IsString(), "bestResponseParams must have a 'EnemyState' string");
        _params.setEnemyInitialState(BOSSParameters::Instance().GetState(brVal["EnemyState"].GetString()));

        BOSS_ASSERT(brVal.HasMember("EnemyBuildOrder") && brVal["EnemyBuildOrder"].IsString(), "BestResponseParams must have a 'EnemyBuildOrder' string");
        _params.setEnemyBuildOrder(BOSSParameters::Instance().GetBuildOrder(brVal["EnemyBuildOrder"].GetString()));
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