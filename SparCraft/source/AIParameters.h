#pragma once

#include "SparCraft.h"
#include <vector>
#include <map>
#include "rapidjson/document.h"

namespace SparCraft
{

class SymStateDesc
{
public:
    std::string mapName;
    size_t borderSize[2];
    size_t centers[2][2];
    std::vector<std::string> unitNames;
    std::vector<size_t> unitCounts;

    SymStateDesc() {}
};


class AIParameters
{   
    rapidjson::Value                        _rootValue;
    
    std::map<std::string, PlayerPtr>        _playerMap[2];
    std::map<std::string, GameState>        _stateMap;
    std::map<std::string, SymStateDesc>     _stateDescMap;

    std::vector<std::string>                _playerNames;
    std::vector<std::string>                _stateNames;

    size_t                                  _playerParses;
    size_t                                  _moveIteratorParses;

    const rapidjson::Value & findPlayer(const std::string & playerName, const rapidjson::Value & rootValue);
    const rapidjson::Value & findState(const std::string & stateName, const rapidjson::Value & rootValue);

    void            parsePlayers(const std::string & keyName, const rapidjson::Value & rootValue);
    void            parseStates(const std::string & keyName, const rapidjson::Value & rootValue);
    
    PlayerPtr       parsePlayer(const size_t & player, const std::string & playerVariable, const rapidjson::Value & root);
    void            parseState(const std::string & stateVariable, const rapidjson::Value & root);
    
    GameState       getStateFromDesc(const SymStateDesc & desc);

    AIParameters();

public:

    static AIParameters & Instance();

    void                parseJSONValue(const rapidjson::Value & rootValue);
    void                parseJSONString(const std::string & jsonString);
    void                parseFile(const std::string & filename);

    PlayerPtr           getPlayer(const size_t & player, const std::string & playerName);
    GameState           getState(const std::string & stateName);

    const std::vector<std::string> & getPlayerNames() const;
    const std::vector<std::string> & getPartialPlayerNames() const;

};
}