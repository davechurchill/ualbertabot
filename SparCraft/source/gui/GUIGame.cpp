#include "GUIGame.h"
#include "GUITools.h"
#include "GUI.h"

using namespace SparCraft;

GLfloat White[4] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat Grid[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

GLfloat PlayerColors[2][4] = {{1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}};
GLfloat PlayerColorsDark[2][4] = {{0.4f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.4f, 0.0f, 1.0f}};

GUIGame::GUIGame(GUI & gui)
    : _game(GameState(), 0)
    , _gui(gui)
    , _updateDelayMS(0)
    , _drawHPBars(true)
    , _drawCD(true)
    , _lastGameUpdate(0)
{
    _updateTimer.start();
}

void GUIGame::onFrame()
{
    //drawHPBars();

    Timer turnTimer;
    turnTimer.start();

    if (!_game.gameOver() && (_updateTimer.getElapsedTimeInMilliSec() > _updateDelayMS))
    {
        _game.playNextTurn();
        _previousTurnTimer =  turnTimer.getElapsedTimeInMilliSec();
        _updateTimer.start();
    }
}

void GUIGame::drawInfo()
{
    std::stringstream ss;
    ss << "Game Frame:  " << _game.getState().getTime() << "\n\n";
    ss << "Render Time: " << _previousDrawGameTimer << "ms\n\n";
    ss << "Player Time: " << _previousTurnTimer << "ms";

    GUITools::DrawString(Position(10, _gui.height()-50), ss.str(), White);
}

void GUIGame::drawEval(int x, int y)
{
    double ltd[2] = { Eval::LTD(_game.getState(), 0), Eval::LTD(_game.getState(), 1) };
    double totalltd[2] = { Eval::TotalLTD(_game.getState(), 0), Eval::TotalLTD(_game.getState(), 1) };
    double ltd2[2] = { Eval::LTD2(_game.getState(), 0), Eval::LTD2(_game.getState(), 1) };
    double totalltd2[2] = { Eval::TotalLTD2(_game.getState(), 0), Eval::TotalLTD2(_game.getState(), 1) };
    double ltdp[2] = { ltd[0] / totalltd[0], ltd[1] / totalltd[1] };
    double ltd2p[2] = { ltd2[0] / totalltd2[0], ltd2[1] / totalltd2[1] };

    Position barSize(150, 20);
    int ySkip = barSize.y() + 2;
    Position ltdpos[2] = { Position(x, y), Position(x, y + ySkip) };

    y += 4*ySkip;
    Position ltd2pos[2] = { Position(x, y), Position(x, y + ySkip) };

    GUITools::DrawString(ltdpos[0] + Position(0, - 2), "LTD Evaluation", White);
    GUITools::DrawString(ltd2pos[0] + Position(0, - 2), "LTD2 Evaluation", White);

    for (size_t p(0); p < 2; ++p)
    {
        GUITools::DrawPercentageRect(ltdpos[p], ltdpos[p] + barSize, ltdp[p], PlayerColors[p], PlayerColorsDark[p]);
        std::stringstream ssltd;
        ssltd << ltd[p];
        GUITools::DrawString(ltdpos[p] + barSize + Position(10, -barSize.y()/4), ssltd.str(), White);

        GUITools::DrawPercentageRect(ltd2pos[p], ltd2pos[p] + barSize, ltd2p[p], PlayerColors[p], PlayerColorsDark[p]);
        std::stringstream ssltd2;
        ssltd2 << ltd2[p];
        GUITools::DrawString(ltd2pos[p] + barSize + Position(10, -barSize.y() / 4), ssltd2.str(), White);
    }
}

void GUIGame::drawGame()
{
    Timer drawGameTimer;
    drawGameTimer.start();

    const GameState & state = _game.getState();

    // draw the map boundary, if the state has one
    if (state.getMap().get() != nullptr)
    {
        int width = state.getMap()->getPixelWidth();
        int height = state.getMap()->getPixelHeight();

        for (size_t x(0); x < state.getMap()->getWalkTileWidth(); ++x)
        {
            for (size_t y(0); y < state.getMap()->getWalkTileHeight(); ++y)
            {
                if (!state.getMap()->isWalkable(x, y))
                {
                    GUITools::DrawRect(BWAPI::Position(x * 8, y * 8), BWAPI::Position((x + 1) * 8, (y + 1) * 8), Grid);
                }
            }
        }

        std::stringstream ssbr;
        ssbr << "(" << width << "," << height << ")";

        GUITools::DrawString(Position(0, -10), "(0,0)", Grid);
        GUITools::DrawString(Position(width - (8 * ssbr.str().size()), height + 15), ssbr.str(), Grid);

        for (int x(32); x < width; x += 32)
        {
            GUITools::DrawLine(Position(x, 0), Position(x, height), 1, Grid);
        }

        for (int y(32); y < height; y += 32)
        {
            GUITools::DrawLine(Position(0, y), Position(width, y), 1, Grid);
        }

        GUITools::DrawLine(Position(0, 0),          Position(width, 0),      3, Grid);
        GUITools::DrawLine(Position(width, 0),      Position(width, height), 3, Grid);
        GUITools::DrawLine(Position(width, height), Position(0, height),     3, Grid);
        GUITools::DrawLine(Position(0, height),     Position(0, 0),          3, Grid);
    }

    for (size_t p(0); p < 2; ++p)
    {
        for (size_t u(0); u < state.numUnits(p); u++)
        {
            drawUnit(state.getUnit(p, u));
        }
    }

    _previousDrawGameTimer = drawGameTimer.getElapsedTimeInMilliSec();
}

void GUIGame::drawParameters(int x, int y)
{
    int size = 11;
    int spacing = 3;
    int colwidth = 175;
    int playerSpacing = 200;

    GUITools::DrawString(Position(x, y), "Player 1 Parameters", PlayerColors[0]);
    GUITools::DrawString(Position(x + playerSpacing, y), "Player 2 Parameters", PlayerColors[1]);

    GUITools::DrawString(Position(x, y+12), _game.getPlayer(0)->getDescription(), White);
    GUITools::DrawString(Position(x + playerSpacing, y + 12), _game.getPlayer(1)->getDescription(), White);
}

void GUIGame::drawSearchResults(int x, int y)
{
	int size = 11;
    int spacing = 3;
    int colwidth = 175;
    int playerSpacing = 200;

    for (size_t pp(0); pp < 2; ++pp)
    {
        GUITools::DrawString(Position(x + pp*playerSpacing, y), "Player Results", PlayerColors[pp]);
        GUITools::DrawString(Position(x + pp*playerSpacing, y+12), _game.getPlayer(pp)->getResultString(), White);
    }
}

void GUIGame::drawUnit(const Unit & unit)
{
    if (!unit.isAlive())
    {
        return;
    }
    
    const int healthBoxHeight = 4;

    const GameState & state = _game.getState();
    const BWAPI::UnitType & type = unit.type();
    const Position pos(unit.currentPosition(state.getTime()));

    _gui.drawUnitType(unit.type(), pos);

    const int x0(pos.x() - type.dimensionLeft());
	const int x1(pos.x() + type.dimensionRight());
	const int y0(pos.y() - type.dimensionUp());
	const int y1(pos.y() + type.dimensionDown());

    double percHP = (double)unit.currentHP() / (double)unit.maxHP();
    int width = x1 - x0;
	int xx = pos.x() - width/2;
	int yy = pos.y() - healthBoxHeight - (y1-y0)/2 - 5;

    GUITools::DrawPercentageRect(Position(xx, yy), Position(xx + x1 - x0, yy + healthBoxHeight), percHP, PlayerColors[unit.getPlayerID()], nullptr);

    size_t cda = unit.nextAttackActionTime() - _game.getState().getTime();
    size_t cdm = unit.nextMoveActionTime() - _game.getState().getTime();
    std::stringstream cdss;
    cdss << "(" << cdm << "," << cda << ")";
    //GUITools::DrawString(Position(xx + width + 5, yy+2), cdss.str(), White);

    const Action & action = unit.previousAction();
            
	if (action.type() == ActionTypes::MOVE)
	{
		glColor4f(1, 1, 1, 0.75);
		glBegin(GL_LINES);
			glVertex2i(pos.x(), pos.y());
			glVertex2i(unit.pos().x(), unit.pos().y());
		glEnd( );
	}
	else if (action.type() == ActionTypes::ATTACK)
	{
		const Unit &	target = state.getUnitByID(action.getTargetID());
		const Position	targetPos(target.currentPosition(state.getTime()));

        GUITools::DrawLine(pos, targetPos, 1, PlayerColors[unit.getPlayerID()]);
	}
}

void GUIGame::setGame(const Game & g)
{
    _game = g;
    _initialState = g.getState();
}

const Game & GUIGame::getGame() const
{
    return _game;
}

void GUIGame::setResults(const size_t & player, const std::vector<std::vector<std::string> > & r)
{
	_results[player] = r;
}

void GUIGame::setParams(const size_t & player, const std::vector<std::vector<std::string> > & p)
{
	_params[player] = p;
}

void GUIGame::setDrawCD(bool draw)
{
    _drawCD = draw;
}

void GUIGame::setUpdateDelayMS(size_t ms)
{
    _updateDelayMS = ms;
}