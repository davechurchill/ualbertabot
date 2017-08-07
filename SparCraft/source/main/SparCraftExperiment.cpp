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

void SparCraftExperiment::parseConfigFile(const std::string & filename)
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
        ss << "Error Substring:  " << json.substr(errorPos-5, 10) << std::endl;

        SPARCRAFT_ASSERT(!parsingFailed, "Error parsing JSON config file: %s", ss.str().c_str());
    }

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

    parseGamesJSON(document["Games"], document);

    printf("Parsing of config file complete\n");
}

void SparCraftExperiment::parseGamesJSON(const rapidjson::Value & games, const rapidjson::Value & root)
{
    Timer t;
    t.start();
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
        
        std::vector<int> winners(3,0);
        for (size_t i(0); i < numGames; ++i)
        {
            //std::cout << "Parsing game " << i << " for " << game["Name"].GetString() << std::endl;
            
            GameState state = ConfigTools::GetStateFromVariable(game["State"].GetString(), root);
            
            PlayerPtr white = AIParameters::Instance().getPlayer(Players::Player_One, game["Players"][0].GetString());
            PlayerPtr black = AIParameters::Instance().getPlayer(Players::Player_Two, game["Players"][1].GetString());

            //std::cout << "Players: " << white->getDescription() << " " << black->getDescription() << "\n\n";

            Game g(state, white, black, 20000);
            playGame(g);

            if (i % 500 == 0)
            {
                double ms = t.getElapsedTimeInMilliSec();
                double gps = (i * 1000.0) / ms;
                //std::cout << "Played game " << i << ", time = " << ms/1000.0 << " @ " << gps << " games per second \n";
            }
        }
    }
}

void SparCraftExperiment::playGame(Game & game)
{
#ifndef SPARCRAFT_NOGUI
    if (_showGUI)
    {
        static GUI gui(_guiWidth, _guiHeight);
        gui.setGame(game);
		gui.setUpdateDelay(_frameDelayMS);

        while (!gui.getGame().gameOver())
        {
            gui.onFrame();
        }
    }
    else
#endif
    {
        game.play();
    }
}
