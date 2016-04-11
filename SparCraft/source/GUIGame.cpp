#include "GUIGame.h"
#include "GUITools.h"
#include "StarCraftGUI.h"

using namespace SparCraft;

GUIGame::GUIGame(StarCraftGUI & gui)
    : _game(GameState(), 0)
    , _gui(gui)
{

}

void GUIGame::onFrame()
{
    drawGame();

    if (!_game.gameOver())
    {
        _game.playNextTurn();
    }
}

void GUIGame::drawGame()
{
    for (size_t p(0); p < 2; ++p)
    {
        for (size_t u(0); u < _game.getState().numUnits(p); u++)
        {
            Unit & unit = _game.getState().getUnit(p, u);

            drawUnit(unit);
        }
    }
}

void GUIGame::drawUnit(const Unit & unit)
{
    _gui.drawUnitType(unit.type(), unit.pos());
}

void GUIGame::setGame(const Game & g)
{
    _game = g;
}

const Game & GUIGame::getGame() const
{
    return _game;
}