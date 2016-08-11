#pragma once

#include "SparCraft.h"
#include <vector>
#include <map>
#include "rapidjson\document.h"

namespace SparCraft
{
class AIParameters
{   
    rapidjson::Value                        _rootValue;
    
    std::map<std::string, PlayerPtr>        _playerMap[2];

    std::vector<std::string>                _playerNames;

    size_t                                  _playerParses;
    size_t                                  _moveIteratorParses;

    const rapidjson::Value & findPlayer(const std::string & playerName, const rapidjson::Value & rootValue);

    void                parsePlayers(const std::string & keyName, const rapidjson::Value & rootValue);
    
    PlayerPtr           parsePlayer(const PlayerID & player, const std::string & playerVariable, const rapidjson::Value & root);
    
    AIParameters();

public:

    static AIParameters & Instance();

    void                parseJSONValue(const rapidjson::Value & rootValue);
    void                parseJSONString(const std::string & jsonString);
    void                parseFile(const std::string & filename);

    PlayerPtr           getPlayer(const PlayerID & player, const std::string & playerName);

    const std::vector<std::string> & getPlayerNames() const;
    const std::vector<std::string> & getPartialPlayerNames() const;

};
}