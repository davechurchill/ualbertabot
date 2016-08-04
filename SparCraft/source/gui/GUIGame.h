#pragma once

#include "../SparCraft.h"

namespace SparCraft
{

class GUI;
class GUIGame
{
	GUI &           _gui;
    Game            _game;
    double          _previousDrawGameTimer;
    double          _previousTurnTimer;
    GameState       _initialState;

    std::vector<std::vector<std::string> > _params[2];
    std::vector<std::vector<std::string> > _results[2];
    std::vector<std::vector<std::string> > _exp;

    void drawGame();
    void drawInfo();
    void drawHPBars();
    void drawUnit(const Unit & unit);
    void drawParameters(int x, int y);
    void drawSearchResults(int x, int y);

    void setResults(const IDType & player, const std::vector<std::vector<std::string> > & r);
    void setParams(const IDType & player, const std::vector<std::vector<std::string> > & p);

public:

    GUIGame(GUI & gui);

    const Game & getGame() const;
    void setGame(const Game & g);
    void onFrame();
};

}
