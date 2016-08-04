#include "GUIGame.h"
#include "GUITools.h"
#include "GUI.h"

using namespace SparCraft;

GLfloat White[4] = {1.0f, 1.0f, 1.0f, 1.0f};

GLfloat PlayerColors[2][4] = {{1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}};
GLfloat PlayerColorsDark[2][4] = {{0.7f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.7f, 0.0f, 1.0f}};

GUIGame::GUIGame(GUI & gui)
    : _game(GameState(), 0)
    , _gui(gui)
{

}

void GUIGame::onFrame()
{
    drawGame();
    drawHPBars();

    Timer turnTimer;
    turnTimer.start();
    if (!_game.gameOver())
    {
        _game.playNextTurn();
        _previousTurnTimer =  turnTimer.getElapsedTimeInMilliSec();

        for (size_t p(0); p < 2; ++p)
        {
            Player_UCT *        uct = dynamic_cast<Player_UCT *>        (_game.getPlayer(p).get());
            Player_AlphaBeta *  ab  = dynamic_cast<Player_AlphaBeta *>  (_game.getPlayer(p).get());

            if (uct) 
            { 
                setParams(p, uct->getParams().getDescription());
                setResults(p, uct->getResults().getDescription());
            }

            if (ab) 
            { 
                setParams(p, ab->getParams().getDescription()); 
                setResults(p, ab->results().getDescription());
            }
        }
    }

    drawParameters(5, 15);
    drawSearchResults(5, 150);
    drawInfo();
}

void GUIGame::drawInfo()
{
    std::stringstream ss;
    ss << "Frame Draw Time: " << _previousDrawGameTimer << "ms\n\n";
    ss << "Turn Time: " << _previousTurnTimer << "ms";

    GUITools::DrawString(Position(5, _gui.height()-20), ss.str(), White);
}

void GUIGame::drawGame()
{
    Timer drawGameTimer;
    drawGameTimer.start();

    const GameState & state = _game.getState();

    for (size_t p(0); p < 2; ++p)
    {
        for (size_t u(0); u < state.numUnits(p); u++)
        {
            drawUnit(state.getUnit(p, u));
        }
    }

    _previousDrawGameTimer = drawGameTimer.getElapsedTimeInMilliSec();
}

void GUIGame::drawHPBars()
{
    const GameState & state = _game.getState();

    for (IDType p(0); p<Constants::Num_Players; ++p)
    {
        for (IDType u(0); u<_initialState.numUnits(p); ++u)
        {
            int barHeight = 12;

            const Unit &			unit(state.getUnitDirect(p,u));

            const Position			pos(1000+170*p,40+barHeight*u);
            const BWAPI::UnitType	type(unit.type());

            const int				x0(pos.x());
            const int				x1(pos.x() + 150);
            const int				y0(pos.y());
            const int				y1(pos.y() + 15);

            // draw the unit HP box
            double	percHP = (double)unit.currentHP() / (double)unit.maxHP();
            int		w = 150;
            int		h = barHeight;
            int		cw = (int)(w * percHP);
            int		xx = pos.x() - w/2;
            int		yy = pos.y() - h - (y1-y0)/2;

            if (unit.isAlive())
            {
                GUITools::DrawRectGradient(Position(xx,yy),Position(xx+cw,yy+h),PlayerColors[p], PlayerColorsDark[p]);
            }

            //if (unit.ID() < 255)
            //{
            //	glEnable( GL_TEXTURE_2D );
            //		glBindTexture( GL_TEXTURE_2D, unit.type().getID() );

            //		// draw the unit to the screen
            //		glColor4f(1, 1, 1, 1);
            //		glBegin( GL_QUADS );
            //			glTexCoord3d(0.0,0.0,.5); glVertex2i(xx, yy);
            //			glTexCoord3d(0.0,1.0,.5); glVertex2i(xx, yy+h);
            //			glTexCoord3d(1.0,1.0,.5); glVertex2i(xx+h,yy+h);
            //			glTexCoord3d(1.0,0.0,.5); glVertex2i(xx+h, yy);
            //		glEnd();
            //	glDisable( GL_TEXTURE_2D );
            //}
        }
    }
}

void GUIGame::drawParameters(int x, int y)
{
    int size = 11;
    int spacing = 3;
    int colwidth = 175;
    int playerspacing = 350;

    for (size_t pp(0); pp < 2; ++pp)
    {
        GUITools::DrawString(Position(x+pp*playerspacing, y), "Player 1 Settings", PlayerColors[pp]);

        for (size_t p(0); _params[pp].size() > 0 && p<_params[pp][0].size(); ++p)
        {
            GUITools::DrawString(Position(x+pp*playerspacing, y+((p+1)*(size+spacing))), _params[pp][0][p], White);
            GUITools::DrawString(Position(x+pp*playerspacing+colwidth, y+((p+1)*(size+spacing))), _params[pp][1][p], White);
        }
    }

    //// Player 1 Settings
    //if (_params[0].size() > 0)
    //{
    //    GUITools::DrawString(Position(x, y), "Player 1 Settings", PlayerColors[0]);

    //    for (size_t p(0); _params[0].size() > 0 && p<_params[0][0].size(); ++p)
    //    {
    //        GUITools::DrawString(Position(x, y+((p+1)*(size+spacing))), _params[0][0][p], White);
    //        GUITools::DrawString(Position(x+colwidth, y+((p+1)*(size+spacing))), _params[0][1][p], White);
    //    }
    //}

    //if (_params[1].size() > 0)
    //{
    //    // Player 2 Settings
    //    x += playerspacing;
    //    glColor3f(0.0, 1.0, 0.0);
    //    DrawText(x, y , size, "Player 2 Settings");
    //    glColor3f(1.0, 1.0, 1.0);

    //    for (size_t p(0); params[1].size() > 0 && p<params[1][0].size(); ++p)
    //    {
    //        DrawText(x, y+((p+1)*(size+spacing)), size, params[1][0][p]);
    //        DrawText(x+colwidth, y+((p+1)*(size+spacing)), size, params[1][1][p]);
    //    }
    //}
}

void GUIGame::drawSearchResults(int x, int y)
{
	int size = 11;
    int spacing = 3;
    int colwidth = 175;
    int playerspacing = 350;

    for (size_t pp(0); pp < 2; ++pp)
    {
        GUITools::DrawString(Position(x+pp*playerspacing, y), "Player 1 Search Results", PlayerColors[pp]);

        for (size_t p(0); _results[pp].size() > 0 && p<_results[pp][0].size(); ++p)
        {
            GUITools::DrawString(Position(x+pp*playerspacing, y+((p+1)*(size+spacing))), _results[pp][0][p], White);
            GUITools::DrawString(Position(x+pp*playerspacing+colwidth, y+((p+1)*(size+spacing))), _results[pp][1][p], White);
        }
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

    const int x0(pos.x() - type.dimensionUp());
	const int x1(pos.x() + type.dimensionDown());
	const int y0(pos.y() - type.dimensionUp());
	const int y1(pos.y() + type.dimensionDown());

    double	percHP = (double)unit.currentHP() / (double)unit.maxHP();
	int		cw = (int)((x1-x0) * percHP);
	int		xx = pos.x() - (x1-x0)/2;
	int		yy = pos.y() - healthBoxHeight - (y1-y0)/2 - 5;

    GUITools::DrawRect(Position(xx, yy), Position(xx+cw, yy+healthBoxHeight), PlayerColors[unit.player()]);

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
		const Unit &	target(state.getUnit(state.getEnemy(unit.player()), action.index()));
		const Position	targetPos(target.currentPosition(state.getTime()));

        GUITools::DrawLine(pos, targetPos, 1, PlayerColors[unit.player()]);
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

void GUIGame::setResults(const IDType & player, const std::vector<std::vector<std::string> > & r)
{
	_results[player] = r;
}

void GUIGame::setParams(const IDType & player, const std::vector<std::vector<std::string> > & p)
{
	_params[player] = p;
}