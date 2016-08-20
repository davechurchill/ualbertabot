#include "Common.h"
#include "AIParameters.h"
#include "AllPlayers.h"

using namespace SparCraft;

AIParameters::AIParameters()
    : _playerParses(0)
    , _moveIteratorParses(0)
{
}

AIParameters & AIParameters::Instance()
{
    static AIParameters params;
    return params;
}

void AIParameters::parseJSONValue(const rapidjson::Value & rootValue)
{  
    Instance() = AIParameters();

    //SPARCRAFT_ASSERT(rootValue.HasMember("Move Iterators"),   "AIParameters: No 'Move Iterators' Options Found");
    SPARCRAFT_ASSERT(rootValue.HasMember("Players"),          "AIParameters: No 'Players' Options Found");

    Timer t;
    t.start();
    
    std::cout << "Parsing Players...\n";
    parsePlayers("Players", rootValue);
    
    std::cout << "Parsing Complete!\n";

    double ms = t.getElapsedTimeInMilliSec();

    std::cout << "Players:            " << _playerMap[0].size()        << ", parses: " << _playerParses << std::endl;
    std::cout << "Parse Time:         " << ms << "ms"                  << std::endl;
}

void AIParameters::parsePlayers(const std::string & keyName, const rapidjson::Value & rootValue)
{
    if (!rootValue.HasMember(keyName.c_str()))
    {
        return;
    }

    const rapidjson::Value & players = rootValue[keyName.c_str()];
    for (rapidjson::Value::ConstMemberIterator itr = players.MemberBegin(); itr != players.MemberEnd(); ++itr)
    {
        const std::string &         name = itr->name.GetString();
        const rapidjson::Value &    val  = itr->value;
        
        SPARCRAFT_ASSERT(val.IsObject(), "Player value must be an Object");

        if (_playerMap[Players::Player_One].find(name) == _playerMap[Players::Player_One].end())
        {
            parsePlayer(Players::Player_One, name, rootValue);
            parsePlayer(Players::Player_Two, name, rootValue);
        }

        _playerNames.push_back(name);
    }

    std::sort(_playerNames.begin(), _playerNames.end());
}

PlayerPtr AIParameters::parsePlayer(const size_t & player, const std::string & playerVariable, const rapidjson::Value & root)
{
    // if the player has already been parsed, no need to re-parse it, just return the already parsed version
    if (_playerMap[player].find(playerVariable) != _playerMap[player].end())
    {
        return getPlayer(player, playerVariable);
    }

    const size_t enemyPlayerID = (player + 1) % 2;

    const rapidjson::Value & playerValue = findPlayer(playerVariable.c_str(), root);

    SPARCRAFT_ASSERT(playerValue.HasMember("Type"), "Player has no 'Type' option");

    std::string playerTypeName = playerValue["Type"].GetString();
    
    PlayerPtr playerPtr;

    if (playerTypeName == "AttackClosest")  
    { 
        playerPtr = PlayerPtr(new Player_AttackClosest(player));
    }
    else if (playerTypeName == "Script")
    { 
        ScriptPlayerPolicy policy(playerValue);

        playerPtr = PlayerPtr(new Player_Script(player, policy));
    }
    else if (playerTypeName == "AlphaBeta")
    {
        playerPtr = PlayerPtr(new Player_AttackClosest(player));
    }
    else if (playerTypeName == "PortfolioGreedySearch")
    {
        SPARCRAFT_ASSERT(playerValue.HasMember("TimeLimit"), "No PGS TimeLimit Found");
        SPARCRAFT_ASSERT(playerValue.HasMember("Iterations"), "No PGS Iterations Found");
        SPARCRAFT_ASSERT(playerValue.HasMember("Responses"), "No PGS Responses Found");
        SPARCRAFT_ASSERT(playerValue.HasMember("EnemySeedPlayer"), "No PGS EnemySeedPlayer Found");
        SPARCRAFT_ASSERT(playerValue.HasMember("Portfolio"), "No PGS Portfolio Found");
        
        PGSParameters params(player);

        params.setEnemySeedPlayer(getPlayer(enemyPlayerID, playerValue["EnemySeedPlayer"].GetString()));
        params.setIterations(playerValue["Iterations"].GetInt());
        params.setResponses(playerValue["Responses"].GetInt());
        params.setTimeLimit(playerValue["TimeLimit"].GetInt());

        const rapidjson::Value & portfolio = playerValue["Portfolio"];

        SPARCRAFT_ASSERT(portfolio.IsArray() && portfolio.Size() > 0, "PGS Portfolio must be a non-empty Array");

        for (size_t i(0); i < portfolio.Size(); ++i)
        {
            params.addPortfolioPlayer(Players::Player_Two, getPlayer(Players::Player_One, portfolio[i].GetString()));
            params.addPortfolioPlayer(Players::Player_Two, getPlayer(Players::Player_Two, portfolio[i].GetString()));
        }

        playerPtr = PlayerPtr(new Player_PortfolioGreedySearch(player, params));
    }
    else if (playerTypeName == "UCT")  
    { 
        const rapidjson::Value & args = playerValue;

        SPARCRAFT_ASSERT(args.HasMember("TimeLimit"), "No UCT TimeLimit Found");
        SPARCRAFT_ASSERT(args.HasMember("MaxTraversals"), "No UCT max traversals Found");
        SPARCRAFT_ASSERT(args.HasMember("MaxChildren"), "No UCT MaxChildren Found");
        SPARCRAFT_ASSERT(args.HasMember("MoveIterator"), "No UCT MoveIterator Found");
        SPARCRAFT_ASSERT(args.HasMember("Eval"), "No UCT EvalType Found");
        
        UCTSearchParameters params;
        params.setMaxPlayer(player);
        params.setTimeLimit(args["TimeLimit"].GetInt());
        params.setMaxTraversals(args["MaxTraversals"].GetInt());
        params.setMaxChildren(args["MaxChildren"].GetInt());


        const std::string & evalMethodString = args["Eval"].GetString();
        if (evalMethodString == "Playout")
        {
            params.setEvalMethod(EvaluationMethods::Playout);

            SPARCRAFT_ASSERT(args.HasMember("PlayoutPlayer"), "No playout player found");

            params.setPlayoutPlayer(Players::Player_One, parsePlayer(Players::Player_One, args["PlayoutPlayer"].GetString(), root));
            params.setPlayoutPlayer(Players::Player_Two, parsePlayer(Players::Player_Two, args["PlayoutPlayer"].GetString(), root));
        }
        else if (evalMethodString == "LTD")
        {
            params.setEvalMethod(EvaluationMethods::LTD);
        }
        else if (evalMethodString == "LTD2")
        {
            params.setEvalMethod(EvaluationMethods::LTD2);
        }
        else
        {
            SPARCRAFT_ASSERT(false, "Unknown UCT Evaluation Method Name: %s", evalMethodString.c_str());
        }
        
        if (args.HasMember("UCTConstant") && args["UCTConstant"].IsDouble())
        {
            params.setCValue(args["UCTConstant"].GetDouble());
        }

        //params.setGraphVizFilename("uct.png");
        
        playerPtr = PlayerPtr(new Player_UCT(player, params));
    }
    else
    {
        SPARCRAFT_ASSERT(false, "Unknown Player Class Name: %s", playerTypeName.c_str());
    }

    _playerParses++;
    playerPtr->setDescription(playerVariable);

    _playerMap[player][playerVariable] = playerPtr->clone();

    return playerPtr->clone();
}
PlayerPtr AIParameters::getPlayer(const size_t & player, const std::string & playerName)
{
    SPARCRAFT_ASSERT(_playerMap[player].find(playerName) != _playerMap[player].end(), "AIParameters::getPlayer Couldn't find player variable: %d %s", (int)_playerMap[player].size(), playerName.c_str());

    return _playerMap[player][playerName]->clone();
}

void AIParameters::parseJSONString(const std::string & jsonString)
{
    rapidjson::Document document;
    bool parsingFailed = document.Parse(jsonString.c_str()).HasParseError();

    if (parsingFailed)
    {
        int errorPos = document.GetErrorOffset();

        std::stringstream ss;
        ss << std::endl << "JSON Parse Error: " << document.GetParseError() << std::endl;
        ss << "Error Position:   " << errorPos << std::endl;
        ss << "Error Substring:  " << jsonString.substr(errorPos-5, 10) << std::endl;

        SPARCRAFT_ASSERT(!parsingFailed, "Error parsing JSON config file: %s", ss.str().c_str());
    }

    parseJSONValue(document);
}

void AIParameters::parseFile(const std::string & filename)
{
    std::cout << "Reading AI Parameters File: " << filename << "\n";
    std::string json = FileUtils::ReadFile(filename);
    
    parseJSONString(json);
}

const std::vector<std::string> & AIParameters::getPlayerNames() const
{
    return _playerNames;
}

const rapidjson::Value & AIParameters::findPlayer(const std::string & playerName, const rapidjson::Value & rootValue)
{
    if (rootValue.HasMember("Players"))
    {
        const rapidjson::Value & players = rootValue["Players"];

        if (players.HasMember(playerName.c_str()))
        {
            return players[playerName.c_str()];
        }
    }

    SPARCRAFT_ASSERT(false, "Player not found: %s", playerName.c_str());
    return rootValue;
}
