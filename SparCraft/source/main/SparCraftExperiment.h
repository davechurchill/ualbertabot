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
    int                                     _guiActionDelay;
    int                                     _guiTurnDelay;

    size_t                                  _currentGameIndex;
    std::vector<Game>                       _games;

    
    std::string         ReadJsonFile(const std::string & filename);

public:

    SparCraftExperiment();
    
    void parseConfigFile(const std::string & filename);
    void parseGamesJSON(const rapidjson::Value & games, const rapidjson::Value & root);
    
    bool hasMoreGames() const;
    const Game & getNextGame();

    bool usingGUI() const;
    int getGuiActionDelay() const;
    int getGuiTurnDelay() const;

    static GameState GetStateFromVariable(const std::string & stateVariable, const rapidjson::Value & root);
    static GameState GetAttackTestState();
    static GameState GetSnipeTestState();
    static GameState GetChillTestState();
    static GameState GetFrontlineTestState();
};
}