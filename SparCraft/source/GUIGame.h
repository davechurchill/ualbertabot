#pragma once

#include "Common.h"
#include "Position.hpp"
#include "Action.h"
#include "Game.h"

namespace SparCraft
{

class StarCraftGUI;
class GUIGame
{
	StarCraftGUI &  _gui;
    Game            _game;

    void drawGame();
    void drawUnit(const Unit & unit);

public:

    GUIGame(StarCraftGUI & gui);

    const Game & getGame() const;
    void setGame(const Game & g);
    void onFrame();
};

}