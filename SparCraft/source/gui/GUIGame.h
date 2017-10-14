#pragma once

#include "../SparCraft.h"

namespace SparCraft
{

class GUI;
class GUIGame
{
	GUI &           _gui;
    Game            _game;
    GameState       _initialState;
    double          _previousDrawGameTimer;
    double          _previousTurnTimer;
    size_t          _updateDelayMS;
    size_t          _lastGameUpdate;
    bool            _drawHPBars;
    bool            _drawCD;
    Timer           _updateTimer;


    std::vector<std::vector<std::string> > _params[2];
    std::vector<std::vector<std::string> > _results[2];
    std::vector<std::vector<std::string> > _exp;

    void drawUnit(const Unit & unit);

    void setResults(const size_t & player, const std::vector<std::vector<std::string> > & r);
    void setParams(const size_t & player, const std::vector<std::vector<std::string> > & p);

public:

    GUIGame(GUI & gui);

    const Game & getGame() const;
    void setGame(const Game & g);
    void onFrame();
    void setDrawCD(bool draw);
    void setUpdateDelayMS(size_t ms);
    void drawParameters(int x, int y);
    void drawSearchResults(int x, int y);
    void drawInfo();
    void drawGame();
    void drawEval(int x, int y);
};

}
