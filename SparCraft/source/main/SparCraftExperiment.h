#pragma once

#include "../SparCraft.h"
#include "../rapidjson/document.h"

namespace SparCraft
{
	struct GameConfiguration
	{
		std::string name;
		GameState state;
		size_t gamesCount;
		std::vector<std::string> players;
	};

class SparCraftExperiment
{   
    std::vector<GameState>                  _initialStates;
    std::map<std::string, std::string>      _partialPlayerDescriptionMap;
    std::map<std::string, std::string>      _playerDescriptionMap;
    std::map<std::string, std::string>      _moveIteratorDescriptionMap;

    bool                                    _showGUI;
	size_t									_frameDelayMS;
    int                                     _guiWidth;
    int                                     _guiHeight;

    std::string         ReadJsonFile(const std::string & filename);

public:

    SparCraftExperiment();
    
	std::vector<GameConfiguration> parseConfigFile(const std::string & filename);
	std::vector<GameConfiguration> parseConfig(const rapidjson::Document& document);
	std::vector<GameConfiguration> parseGamesJSON(const rapidjson::Value & games, const rapidjson::Value & root);
    void playGame(Game & game);
	void runExperiement(const GameConfiguration& configuration, AIParameters & parameters);
#ifndef SPARCRAFT_NOGUI
	void playGameWithUI(Game & game, std::string assetsFolder);
	void runExperiementWithUI(const GameConfiguration& configuration, std::string assetsFolder, AIParameters & parameters);
#endif
};
}