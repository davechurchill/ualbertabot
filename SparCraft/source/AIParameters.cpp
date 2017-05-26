#include "Common.h"
#include "AIParameters.h"
#include "AllPlayers.h"
#include "torch/TorchTools.h"

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
    SPARCRAFT_ASSERT(rootValue.HasMember("Engine"),  "AIParameters: No 'Engine' Options Found");
    SPARCRAFT_ASSERT(rootValue.HasMember("Players"), "AIParameters: No 'Players' Options Found");

    Timer t;
    t.start();
    
    const rapidjson::Value & engine = rootValue["Engine"];
    SPARCRAFT_ASSERT(engine.HasMember("GroundUnitMovePenalty") && engine["GroundUnitMovePenalty"].IsInt(), "Engine params need GroundUnitMovePenalty int");
    SPARCRAFT_ASSERT(engine.HasMember("AirUnitMovePenalty") && engine["AirUnitMovePenalty"].IsInt(), "Engine params need GroundUnitMovePenalty int");
    SPARCRAFT_ASSERT(engine.HasMember("UnitRangeAddition") && engine["UnitRangeAddition"].IsInt(), "Engine params need UnitRangeAddition int");
    SPARCRAFT_ASSERT(engine.HasMember("UnitMoveAfterAttackBuffer") && engine["UnitMoveAfterAttackBuffer"].IsInt(), "Engine params need UnitMoveAfterAttackBuffer int");

    Config::Units::GroundUnitMovePenalty        = engine["GroundUnitMovePenalty"].GetInt();
    Config::Units::AirUnitMovePenalty           = engine["AirUnitMovePenalty"].GetInt();
    Config::Units::UnitRangeAddition            = engine["UnitRangeAddition"].GetInt();
    Config::Units::UnitMoveAfterAttackBuffer    = engine["UnitMoveAfterAttackBuffer"].GetInt();

    //std::cout << "Parsing Players...\n";
    parsePlayers("Players", rootValue);
    //parseStates("States", rootValue);
    
    //std::cout << "Parsing Complete!\n";

    double ms = t.getElapsedTimeInMilliSec();

    //std::cout << "Players:            " << _playerMap[0].size()        << ", parses: " << _playerParses << std::endl;
    //std::cout << "Parse Time:         " << ms << "ms"                  << std::endl;
}

void AIParameters::parsePlayers(const std::string & keyName, const rapidjson::Value & rootValue)
{
    if (!rootValue.HasMember(keyName.c_str()))
    {
        return;
    }

    const rapidjson::Value & players = rootValue[keyName.c_str()];
    for (auto itr = players.MemberBegin(); itr != players.MemberEnd(); ++itr)
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

void AIParameters::parseStates(const std::string & keyName, const rapidjson::Value & rootValue)
{
    if (!rootValue.HasMember(keyName.c_str()))
    {
        return;
    }

    const rapidjson::Value & states = rootValue[keyName.c_str()];
    for (auto itr = states.MemberBegin(); itr != states.MemberEnd(); ++itr)
    {
        const std::string &         name = itr->name.GetString();
        const rapidjson::Value &    val = itr->value;

        SPARCRAFT_ASSERT(val.IsObject(), "State value must be an Object");

        parseState(name, rootValue);
        _stateNames.push_back(name);
    }

    std::sort(_stateNames.begin(), _stateNames.end());
}

void AIParameters::parseState(const std::string & stateVariable, const rapidjson::Value & root)
{
    SPARCRAFT_ASSERT(root["States"].HasMember(stateVariable.c_str()), "State variable not found");

    const rapidjson::Value & stateValue = root["States"][stateVariable.c_str()];
    SPARCRAFT_ASSERT(stateValue.HasMember("Type"), "State has no 'Type' option");
    const std::string & stateType = stateValue["Type"].GetString();

    GameState state;
    if (stateType == "Symmetric")
    {
        SPARCRAFT_ASSERT(stateValue.HasMember("Border") && stateValue["Border"].IsArray(), "Separated State required 'Border' array");
        SPARCRAFT_ASSERT(stateValue.HasMember("Centers") && stateValue["Centers"].IsArray(), "Separated State required 'Centers' array");
        SPARCRAFT_ASSERT(stateValue.HasMember("Units") && stateValue["Units"].IsArray(), "Separated State required 'Units' array");

        SymStateDesc desc;

        // parse the border
        const rapidjson::Value & borderValue = stateValue["Border"];
        SPARCRAFT_ASSERT(borderValue.Size() == 2 && borderValue[0].IsInt() && borderValue[1].IsInt(), "Separated State 'Border' must be array of size 2 [int, int]");
        
        desc.borderSize[0] = borderValue[0].GetInt();
        desc.borderSize[1] = borderValue[1].GetInt();

        // parse the centers
        const rapidjson::Value & centers = stateValue["Centers"];
        SPARCRAFT_ASSERT(centers.Size() == 2 && centers[0].IsArray() && centers[1].IsArray(), "Separated State 'Centers' must be array of 2 arrays of 2 ints each [[int,int],[int][int]]");
        
        desc.centers[0][0] = centers[0][0].GetInt();
        desc.centers[0][1] = centers[0][1].GetInt();
        desc.centers[1][0] = centers[1][0].GetInt();
        desc.centers[1][1] = centers[1][1].GetInt();

        // parse the units
        const rapidjson::Value & units = stateValue["Units"];
        for (size_t i(0); i < units.Size(); ++i)
        {
            SPARCRAFT_ASSERT(units[i].IsArray() && units[i].Size() == 2, "Units array member must be array of size 2");
            SPARCRAFT_ASSERT(units[i][0].IsString() && units[i][1].IsInt(), "Unis array member must be [UnitType (String), Number (int)]");

            desc.unitNames.push_back(units[i][0].GetString());
            desc.unitCounts.push_back(units[i][1].GetInt());
        };

        if (stateValue.HasMember("Map") && stateValue["Map"].IsString())
        {
            desc.mapName = stateValue["Map"].GetString();
        }

        _stateDescMap[stateVariable] = desc;
        return;
    }
    else if (stateType == "StateFile")
    {
        SPARCRAFT_ASSERT(stateValue.HasMember("File") && stateValue["File"].IsString(), "StateFile must have 'File' String member");

        std::ifstream fin(stateValue["File"].GetString());

        SPARCRAFT_ASSERT(fin.good(), "Couldn't open State File: %s", stateValue["File"].GetString());

        GameState state;
        std::string unitType;
        size_t player = 0, posX = 0, posY = 0;

        while (fin >> unitType >> player >> posX >> posY)
        {
            Unit unit(BWAPI::UnitType::getType(unitType), player, Position(posX, posY));
            state.addUnit(unit);
        }
    }
    else if (stateType == "TorchCraftFrame")
    {
        SPARCRAFT_ASSERT(stateValue.HasMember("File") && stateValue["File"].IsString(), "TorchCraftFrame must have 'File' String member");

        state = TorchTools::GetSparCraftStateFromTorchCraftFrameFile(stateValue["File"].GetString());
    }
    else if (stateType == "TorchCraftStdIn")
    {
        SPARCRAFT_ASSERT(stateValue.HasMember("File") && stateValue["File"].IsString(), "TorchCraftFrame must have 'File' String member");

        std::ifstream fin(stateValue["File"].GetString());

        SPARCRAFT_ASSERT(fin.good(), "Couldn't open file: %s", stateValue["File"].GetString());

        std::string type, aiPlayerName;
        int playerID = 0, mapWidth = 0, mapHeight = 0;

        fin >> type >> aiPlayerName >> playerID >> mapWidth >> mapHeight;

        GameState state = TorchTools::GetSparCraftStateFromTorchCraftFrameStream(fin);
        state.setMap(std::shared_ptr<Map>(new Map(mapWidth / 4, mapHeight / 4)));
    }
    else
    {
        SPARCRAFT_ASSERT(false, "Unknown state type: %s", stateType.c_str());
    }

    _stateMap[stateVariable] = state;
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
        SPARCRAFT_ASSERT(playerValue.HasMember("OutOfRange"), "Script Policy has no 'OutOfRange' member");
        SPARCRAFT_ASSERT(playerValue.HasMember("InRange"), "Script Policy has no 'InRange' member");
        SPARCRAFT_ASSERT(playerValue.HasMember("Reload"), "Script Policy has no 'Reload' member");
        SPARCRAFT_ASSERT(playerValue.HasMember("NOK"), "Script Policy has no 'NOK' member");

        ScriptPolicy oor, ir, reload;

        const rapidjson::Value & oorValue = playerValue["OutOfRange"].IsString() ? findPolicy(playerValue["OutOfRange"].GetString(), root) : playerValue["OutOfRange"];
        const rapidjson::Value & irValue = playerValue["InRange"].IsString() ? findPolicy(playerValue["InRange"].GetString(), root) : playerValue["InRange"];
        const rapidjson::Value & reloadValue = playerValue["Reload"].IsString() ? findPolicy(playerValue["Reload"].GetString(), root) : playerValue["Reload"];
        bool nok = !playerValue["NOK"].GetBool();

        ScriptPlayerPolicy policy(ScriptPolicy(oorValue), ScriptPolicy(irValue), ScriptPolicy(reloadValue), nok);

        playerPtr = PlayerPtr(new Player_Script(player, policy));
    }
    else if (playerTypeName == "AlphaBeta")
    {
        playerPtr = PlayerPtr(new Player_AttackClosest(player));
    }
    else if (playerTypeName == "PortfolioGreedySearch")
    {
        SPARCRAFT_ASSERT(playerValue.HasMember("TimeLimit"), "No PGS TimeLimit Found");
        SPARCRAFT_ASSERT(playerValue.HasMember("MaxPlayoutTurns"), "No PGS MaxPlayoutTurns Found");
        SPARCRAFT_ASSERT(playerValue.HasMember("Iterations"), "No PGS Iterations Found");
        SPARCRAFT_ASSERT(playerValue.HasMember("Responses"), "No PGS Responses Found");
        SPARCRAFT_ASSERT(playerValue.HasMember("EnemySeedPlayer"), "No PGS EnemySeedPlayer Found");
        SPARCRAFT_ASSERT(playerValue.HasMember("Portfolio"), "No PGS Portfolio Found");
        
        PGSParameters params(player);

        params.setEnemySeedPlayer(getPlayer(enemyPlayerID, playerValue["EnemySeedPlayer"].GetString()));
        params.setIterations(playerValue["Iterations"].GetInt());
        params.setResponses(playerValue["Responses"].GetInt());
        params.setTimeLimit(playerValue["TimeLimit"].GetInt());
        params.setMaxPlayoutTurns(playerValue["MaxPlayoutTurns"].GetInt());

        const rapidjson::Value & portfolio = playerValue["Portfolio"];

        SPARCRAFT_ASSERT(portfolio.IsArray() && portfolio.Size() > 0, "PGS Portfolio must be a non-empty Array");

        for (size_t i(0); i < portfolio.Size(); ++i)
        {
            params.addPortfolioPlayer(Players::Player_One, getPlayer(Players::Player_One, portfolio[i].GetString()));
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
    playerPtr->setName(playerVariable);

    _playerMap[player][playerVariable] = playerPtr->clone();

    return playerPtr->clone();
}

GameState AIParameters::getState(const std::string & stateName)
{
    if (_stateMap.find(stateName) != _stateMap.end())
    {
        return _stateMap[stateName];
    }
    else if (_stateDescMap.find(stateName) != _stateDescMap.end())
    {
        return getStateFromDesc(_stateDescMap[stateName]);
    }
    else
    {
        SPARCRAFT_ASSERT(false, "Couldn't find state variable: %s", stateName.c_str());
    }

    return GameState();
}

GameState AIParameters::getStateFromDesc(const SymStateDesc & desc)
{
    GameState state;

    for (size_t u(0); u < desc.unitNames.size(); ++u)
    {
        BWAPI::UnitType type = BWAPI::UnitType::getType(desc.unitNames[u]);

        // add the symmetric unit for each count in the numUnits Vector
        for (size_t n(0); n < desc.unitCounts[u]; ++n)
        {
            Position r((rand() % (2 * desc.borderSize[0])) - desc.borderSize[0], (rand() % (2 * desc.borderSize[1])) - desc.borderSize[1]);
            Position u1(desc.centers[0][0] + r.x(), desc.centers[0][1] + r.y());
            Position u2(desc.centers[1][0] - r.x(), desc.centers[1][1] - r.y());

            state.addUnit(SparCraft::Unit(type, Players::Player_One, u1));
            state.addUnit(SparCraft::Unit(type, Players::Player_Two, u2));
        }
    }

    return state;
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
    //std::cout << "Reading AI Parameters File: " << filename << "\n";
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

const rapidjson::Value & AIParameters::findPolicy(const std::string & policyName, const rapidjson::Value & rootValue)
{
    if (rootValue.HasMember("Policies"))
    {
        const rapidjson::Value & policies = rootValue["Policies"];

        if (policies.HasMember(policyName.c_str()))
        {
            return policies[policyName.c_str()];
        }
    }

    SPARCRAFT_ASSERT(false, "Policy not found: %s", policyName.c_str());
    return rootValue;
}
