#pragma once

#include "../SparCraft.h"
#include "../rapidjson/document.h"

namespace SparCraft
{
class SparCraftExperiment
{   
    std::vector<GameState>                  _initialStates;
    std::map<std::string, std::string>      _partialPlayerDescriptionMap;
    std::map<std::string, std::string>      _playerDescriptionMap;
    std::map<std::string, std::string>      _moveIteratorDescriptionMap;

    bool                                    _showGUI;
    int                                     _guiWidth;
    int                                     _guiHeight;

    std::string         ReadJsonFile(const std::string & filename);

public:

    SparCraftExperiment();
    
    void parseConfigFile(const std::string & filename);
    void parseGamesJSON(const rapidjson::Value & games, const rapidjson::Value & root);
    void playGame(Game & game);
    
    static GameState GetStateFromVariable(const std::string & stateVariable, const rapidjson::Value & root);
};
}