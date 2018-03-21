#include "SparCraftExperiment.h"
#include "../torch/TorchTools.h"
#include "../ConfigTools.h"

// #define SPARCRAFT_NOGUI

#ifndef SPARCRAFT_NOGUI
    #include "../gui/GUI.h"
#endif

using namespace SparCraft;

SparCraftExperiment::SparCraftExperiment()
    : _showGUI(false)
    , _frameDelayMS(0)
    , _guiWidth(1280)
    , _guiHeight(720)
{

}

std::vector<GameConfiguration> SparCraftExperiment::parseConfigFile(const std::string & filename)
{
	rapidjson::Document document;

	std::string json = FileUtils::ReadFile(filename);
	bool parsingFailed = document.Parse(json.c_str()).HasParseError();

	if (parsingFailed)
	{
		int errorPos = document.GetErrorOffset();

		std::stringstream ss;
		ss << std::endl << "JSON Parse Error: " << document.GetParseError() << std::endl;
		ss << "Error Position:   " << errorPos << std::endl;
		ss << "Error Substring:  " << json.substr(errorPos - 5, 10) << std::endl;

		SPARCRAFT_ASSERT(!parsingFailed, "Error parsing JSON config file: %s", ss.str().c_str());
	}

	return parseConfig(document);
}

std::vector<GameConfiguration> SparCraftExperiment::parseConfig(const rapidjson::Document& document)
{
    SPARCRAFT_ASSERT(document.HasMember("GUI"),              "No 'GUI' Options Found");
    SPARCRAFT_ASSERT(document.HasMember("States"),           "No 'States' Found");
    SPARCRAFT_ASSERT(document.HasMember("Games"),            "No 'Games' Options Found");

    const rapidjson::Value & guiValue = document["GUI"];
    SPARCRAFT_ASSERT(guiValue.HasMember("Enabled"), "GUI has no 'Enabled' option");
    SPARCRAFT_ASSERT(guiValue.HasMember("Width"), "GUI has no 'Width' option");
    SPARCRAFT_ASSERT(guiValue.HasMember("Height"), "GUI has no 'Height' option");
    SPARCRAFT_ASSERT(guiValue.HasMember("FrameDelayMS"), "GUI has no 'FrameDelayMS' option");

    _showGUI = guiValue["Enabled"].GetBool();
    _guiWidth = guiValue["Width"].GetInt();
    _guiHeight = guiValue["Height"].GetInt();
    _frameDelayMS = guiValue["FrameDelayMS"].GetInt();

    auto result = parseGamesJSON(document["Games"], document);

    printf("Parsing of config file complete\n");
	return result;
}

std::vector<GameConfiguration> SparCraftExperiment::parseGamesJSON(const rapidjson::Value & games, const rapidjson::Value & root)
{
	std::vector<GameConfiguration> result;
    for (size_t gg(0); gg<games.Size(); ++gg)
    {
        const rapidjson::Value & game = games[gg];

        // if we don't want to play this set of games, just skip it
        if (!game.HasMember("Play") || (game["Play"].IsBool() && !game["Play"].GetBool()))
        {
            continue;
        }

        SPARCRAFT_ASSERT(game.HasMember("Name"), "Game has no 'Name' option");
        SPARCRAFT_ASSERT(game.HasMember("State") && game["State"].IsString(), "Game has no 'State' String option");
        SPARCRAFT_ASSERT(game.HasMember("Games") && game["Games"].IsInt(), "Game has no 'Gtate' int option");
        SPARCRAFT_ASSERT(game.HasMember("Players") && game["Players"].IsArray(), "Game has no 'Players' array option");

        size_t numGames = game["Games"].GetInt();
		GameConfiguration gameConfiguration;
		gameConfiguration.name = game["Name"].GetString();
		gameConfiguration.state = ConfigTools::GetStateFromVariable(game["State"].GetString(), root);
		gameConfiguration.gamesCount = numGames;
		auto& playersConfiguration = game["Players"];
		for (rapidjson::SizeType i = 0; i < playersConfiguration.Size(); i++)
		{
			gameConfiguration.players.push_back(playersConfiguration[i].GetString());
		}

		result.push_back(gameConfiguration);
    }

	return result;
}

void SparCraftExperiment::runExperiement(const GameConfiguration& configuration, AIParameters & parameters)
{
	for (size_t i = 0; i < configuration.gamesCount; ++i)
	{
		PlayerPtr white = parameters.getPlayer(Players::Player_One, configuration.players[0]);
		PlayerPtr black = parameters.getPlayer(Players::Player_Two, configuration.players[1]);
		Game g(configuration.state, white, black, 20000);
		playGame(g);
	}
}

void SparCraftExperiment::playGame(Game & game)
{
	game.play();
}

#ifndef SPARCRAFT_NOGUI
void SparCraftExperiment::runExperiementWithUI(const GameConfiguration& configuration, std::string assetsFolder, AIParameters & parameters)
{
	for (size_t i = 0; i < configuration.gamesCount; ++i)
	{
		PlayerPtr white = parameters.getPlayer(Players::Player_One, configuration.players[0]);
		PlayerPtr black = parameters.getPlayer(Players::Player_Two, configuration.players[1]);

		Game g(configuration.state, white, black, 20000);
		playGameWithUI(g, assetsFolder);
	}
}

void SparCraftExperiment::playGameWithUI(Game & game, std::string assetsFolder)
{
	if (_showGUI)
	{
		static GUI gui(_guiWidth, _guiHeight, assetsFolder);
		gui.setGame(game);
		gui.setUpdateDelay(_frameDelayMS);

		while (!gui.getGame().gameOver())
		{
			gui.onFrame();
		}
	}
}
#endif
