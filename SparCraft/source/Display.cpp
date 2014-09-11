#include "Display.h"

#ifdef USING_VISUALIZATION_LIBRARIES

using namespace SparCraft;

#ifdef WIN32
    // disable vsync in windows
    #include "glext/wglext.h"
    #include "glext/glext.h"
    #include "glfont/glfont.h"
    typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
    PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
#endif

PixelPerfectGLFont font;
GLuint fontID = 0;

Display::Display(const int mw, const int mh) 
	: windowSizeX(1280)
	, windowSizeY(720)
	, mapWidth(mw)
	, mapHeight(mh)
	, bl(false)
	, br(false)
	, bu(false)
	, bd(false)
	, zoomX(0)
	, zoomY(0)
	, drawResults(false)
	, started(false)
{
	playerTypes[0] = 0;
	playerTypes[1] = 0;

	textureSizes = std::vector<Position>(BWAPI::UnitTypes::allUnitTypes().size() + 10);

	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		throw std::runtime_error("Unable to initialise SDL");
	}
}

Display::~Display()
{
	SDL_Quit();
}

void Display::OnStart()
{
	if (started)
	{
		return;
	}

	mapPixelWidth	= mapWidth * 32;
	mapPixelHeight	= mapHeight * 32;

	cameraX			= 400 - windowSizeX / 2;
	cameraY			= 400 - windowSizeY / 2;
    
#ifdef WIN32
    PROC p = wglGetProcAddress("wglSwapIntervalEXT");
#endif

    //(*wglGetProcAddress("wglSwapIntervalEXT"));

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
	screen = SDL_SetVideoMode(windowSizeX, windowSizeY, 32, SDL_OPENGL);
	SDL_WM_SetCaption("SparCraft Visualization", 0);


	// enable alpha blending for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );
	
	// load the unit textures for later use
	LoadTextures();

    // load the font
    std::stringstream fontFile;
#ifdef WIN32
    fontFile << imageDir << "fonts\\couriernew.glf";
#else
    fontFile << imageDir << "fonts/couriernew.glf";
#endif
    font.Create(fontFile.str(), 99);

#ifdef WIN32
    setVSync(0);
#endif
	started = true;
}

void Display::SetImageDir(const std::string & filename)
{
    imageDir = filename;
}

#ifdef WIN32
void Display::setVSync(int interval)
{
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

	if( wglSwapIntervalEXT )
    {
		if(wglSwapIntervalEXT(interval))
        {
			//puts("VSync changed");
		} 
        else 
        {
			//puts("VSync change failed");
		}
	}
    else 
    {
		//puts("VSync change unsupported");
	} 
}
#endif

void Display::SetState(const GameState & s)
{
	state = s;
}

void Display::SetResults(const IDType & player, const std::vector<std::vector<std::string> > & r)
{
	results[player] = r;
}

void Display::SetParams(const IDType & player, const std::vector<std::vector<std::string> > & p)
{
	params[player] = p;
}

void Display::SetExpDesc(const std::vector<std::vector<std::string> > & d)
{
	exp = d;
}

void Display::SetPlayerTypes(const IDType & p1, const IDType & p2)
{
	playerTypes[0] = p1;
	playerTypes[1] = p2;
}

void Display::OnFrame()
{
	// Handle input events
	HandleEvents();

	// Render the frame
	glClear(GL_COLOR_BUFFER_BIT);
	RenderMainMap();
	RenderMinimap();
	RenderTextOverlay();
	RenderInformation();
	
	SDL_GL_SwapBuffers();
}

void Display::HandleEvents()
{
	// Handle SDL events
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				const bool pressed(event.key.state == SDL_PRESSED);
				switch(event.key.keysym.sym)
				{
					case SDLK_LEFT:		bl = pressed; break;
					case SDLK_RIGHT:	br = pressed; break;
					case SDLK_UP:		bu = pressed; break;
					case SDLK_DOWN:		bd = pressed; break;
				}
			}
			break;
			case SDL_MOUSEBUTTONDOWN:
			{
				const int2 minimapSize(mapWidth,mapHeight);
				const int2 minimapLocation(0,windowSizeY-minimapSize.y);
				const int2 mouse(event.button.x - minimapLocation.x, event.button.y - minimapLocation.y);
				if(mouse.x >= 0 && mouse.x < mapWidth && mouse.y >= 0 && mouse.y < mapHeight)
				{
					cameraX = mouse.x * 32 - windowSizeX/2;
					cameraY = mouse.y * 32 - windowSizeY/2;
				}

				if(event.button.button == SDL_BUTTON_WHEELUP)
				{
					zoomX += 128;
					zoomY += 72;
				}
				else if(event.button.button == SDL_BUTTON_WHEELDOWN)
				{
					zoomX -= 128;
					zoomY -= 72;
				}
			}
			break;
			case SDL_QUIT:
                System::FatalError("Visualization Shut Down");
		}
	}

	// Move the camera
	const int cameraSpeed(32);
	if(bl) cameraX -= cameraSpeed;
	if(br) cameraX += cameraSpeed;
	if(bu) cameraY -= cameraSpeed;
	if(bd) cameraY += cameraSpeed;

	cameraX = std::max(0, std::min(cameraX, mapPixelWidth-windowSizeX));
	cameraY = std::max(0, std::min(cameraY, mapPixelHeight-windowSizeY));
}

void Display::RenderMainMap()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		{
			glOrtho(0,windowSizeX-zoomX,windowSizeY-zoomY,0,-1,1);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			{
				glTranslatef(static_cast<float>(-cameraX),static_cast<float>(-cameraY),0);

				const int vx0(cameraX), vx1(cameraX+windowSizeX-zoomX);
				const int vy0(cameraY), vy1(cameraY+windowSizeY-zoomY);

				RenderTerrain(	std::max(vx0>>3,0), std::max(vy0>>3,0), 
					std::min((vx1>>3) + 1, mapWidth*4),
					std::min((vy1>>3) + 1, mapHeight*4));
				RenderUnits();

				BOOST_FOREACH (const Shape & shape, shapes)
				{
					shape.OnRender();
				}
			}
			

			glPopMatrix();
		}
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}
	glPopAttrib();
}

void Display::RenderMinimap()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glOrtho(0,windowSizeX,0,windowSizeY,-1,1);
	glBegin(GL_QUADS);
		glVertex2i(0,0);
		glVertex2i(mapWidth+1,0);
		glVertex2i(mapWidth+1,mapHeight+1);
		glVertex2i(0,mapHeight+1);
	glEnd();

	glLoadIdentity();
	glOrtho(0,mapPixelWidth,mapPixelHeight,0,-1,1);
	glViewport(0,0,mapWidth,mapHeight);

	RenderTerrain(0,0,mapWidth*4,mapHeight*4);

	BOOST_FOREACH (const Shape & shape, shapes)
	{
		shape.OnRender();
	}

	RenderUnits();

	// Render outline around viewport on minimap
	{
		const int vpx0(cameraX), vpx1(vpx0 + windowSizeX - zoomX);
		const int vpy0(cameraY), vpy1(vpy0 + windowSizeY - zoomY);
		
		glColor3f(1,1,1);
		glBegin(GL_LINE_STRIP);
			glVertex2i(vpx0,vpy0);
			glVertex2i(vpx1,vpy0);
			glVertex2i(vpx1,vpy1);
			glVertex2i(vpx0,vpy1);
			glVertex2i(vpx0,vpy0);
		glEnd();
	}

	glPopMatrix();
	glPopAttrib();
}

void Display::RenderTextOverlay()
{
	int size = 15;
    /*
	std::stringstream ss;
	ss << "Game Frame: " << state.getTime();

	std::stringstream ss2;
	ss2 << "Game Seconds: " << state.getTime() / 24;

	glColor3f(0.0, 1.0, 0.0);
	DrawText(700, 5     , size, "Game Time");

	glColor3f(1.0, 1.0, 1.0);
	DrawText(700, 20, size, ss.str());
	DrawText(700, 35, size, ss2.str());*/
    
	DrawParameters(5, 3);
    DrawSearchResults(5, 150);
    DrawExperiment(675, 5);
}

void Display::DrawSearchResults(int x, int y)
{
	int size = 11;
    int spacing = 3;
    int colwidth = 175;
    int playerspacing = 350;

    // Player 1 Settings
    // Player 2 Settings
    if (results[0].size() > 0)
    {
        glColor3f(1.0, 0.0, 0.0);
        DrawText(x, y , size, "Player 1 Search Results");
        glColor3f(1.0, 1.0, 1.0);

        for (size_t r(0); results[0].size() > 0 && r<results[0][0].size(); ++r)
        {
            DrawText(x, y+((r+1)*(size+spacing)), size, results[0][0][r]);
            DrawText(x+colwidth, y+((r+1)*(size+spacing)), size, results[0][1][r]);
        }
    }

    // Player 2 Settings
    if (results[1].size() > 0)
    {
        x += playerspacing;
        glColor3f(0.0, 1.0, 0.0);
        DrawText(x, y , size, "Player 2 Search Results");
        glColor3f(1.0, 1.0, 1.0);

        for (size_t r(0); results[1].size() > 0 && r<results[1][0].size(); ++r)
        {
            DrawText(x, y+((r+1)*(size+spacing)), size, results[1][0][r]);
            DrawText(x+colwidth, y+((r+1)*(size+spacing)), size, results[1][1][r]);
        }
    }
}

void Display::DrawParameters(int x, int y)
{
    int size = 11;
    int spacing = 3;
    int colwidth = 175;
    int playerspacing = 350;

    // Player 1 Settings
    if (params[0].size() > 0)
    {
        glColor3f(1.0, 0.0, 0.0);
        DrawText(x, y , size, "Player 1 Settings");
        glColor3f(1.0, 1.0, 1.0);

        for (size_t p(0); params[0].size() > 0 && p<params[0][0].size(); ++p)
        {
            DrawText(x, y+((p+1)*(size+spacing)), size, params[0][0][p]);
            DrawText(x+colwidth, y+((p+1)*(size+spacing)), size, params[0][1][p]);
        }
    }

    if (params[1].size() > 0)
    {
        // Player 2 Settings
        x += playerspacing;
        glColor3f(0.0, 1.0, 0.0);
        DrawText(x, y , size, "Player 2 Settings");
        glColor3f(1.0, 1.0, 1.0);

        for (size_t p(0); params[1].size() > 0 && p<params[1][0].size(); ++p)
        {
            DrawText(x, y+((p+1)*(size+spacing)), size, params[1][0][p]);
            DrawText(x+colwidth, y+((p+1)*(size+spacing)), size, params[1][1][p]);
        }
    }
}

void Display::DrawExperiment(int x, int y)
{
    int size = 11;
    int spacing = 3;
    int colwidth = 100;
    int playerspacing = 350;

    std::stringstream ss;
    ss << "Game Frame: " << state.getTime();

    glColor3f(1.0, 0.0, 0.0);
    DrawText(x, y , size, ss.str());
    glColor3f(1.0, 1.0, 1.0);

    for (size_t p(0); p<exp[0].size(); ++p)
    {
        DrawText(x, y+((p+1)*(size+spacing)), size, exp[0][p]);
        DrawText(x+colwidth, y+((p+1)*(size+spacing)), size, exp[1][p]);
    }
}

void Display::RenderInformation()
{
	static const float3 factionColors[12] = 
	{
		float3(1,0,0),		float3(0,1,0),			float3(0,1,0.5f),	float3(0.5f,0,1),
		float3(1,0.5f,0),	float3(0.5f,0.25f,0),	float3(1,1,1),		float3(1,1,0),
		float3(0,0,0),		float3(0,0,0),			float3(0,0,0),		float3(0,1,1)
	};

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		{
			glOrtho(0,windowSizeX,windowSizeY,0,-1,1);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			{
				for (IDType p(0); p<Constants::Num_Players; ++p)
				{
					for (IDType u(0); u<Constants::Max_Units; ++u)
					{
                        int barHeight = 12;

						const Unit &			unit(state.getUnitDirect(p, u));

						const Position			pos(1000+170*p, 40+barHeight*u);
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
							glColor4f(factionColors[p].x, factionColors[p].y, factionColors[p].z, 0.75);
							glBegin(GL_QUADS);
								glVertex2i(xx,yy);
								glVertex2i(xx+cw,yy);
								glColor4f(0.2f, 0.2f, 0.2f, 0.75);
								glVertex2i(xx+cw,yy+h);
								glColor4f(factionColors[p].x, factionColors[p].y, factionColors[p].z, 0.75);
								glVertex2i(xx,yy+h);
							glEnd();
						}

						if (unit.ID() < 255)
						{
							glEnable( GL_TEXTURE_2D );
								glBindTexture( GL_TEXTURE_2D, unit.type().getID() );

								// draw the unit to the screen
								glColor4f(1, 1, 1, 1);
								glBegin( GL_QUADS );
									glTexCoord3d(0.0,0.0,.5); glVertex2i(xx, yy);
									glTexCoord3d(0.0,1.0,.5); glVertex2i(xx, yy+h);
									glTexCoord3d(1.0,1.0,.5); glVertex2i(xx+h,yy+h);
									glTexCoord3d(1.0,0.0,.5); glVertex2i(xx+h, yy);
								glEnd();
							glDisable( GL_TEXTURE_2D );
						}
					}
				}
				
			}
			glPopMatrix();
		}
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}
	glPopAttrib();
}

void Display::RenderTerrain(int wx0, int wy0, int wx1, int wy1)
{
	int dX = 1280;
	int dY = 800;

	for (int x(0); x<mapPixelWidth; x+=dX)
	{
		for (int y(0); y<mapPixelHeight; y+=dY)
		{
			//glBegin(GL_QUADS);
			glEnable( GL_TEXTURE_2D );
				// bind the correct tecture based on this unit
				glBindTexture( GL_TEXTURE_2D, 4 );
				
				// draw the unit to the screen
				glColor4f(1.0, 1.0, 1.0, 1.0);
				glBegin( GL_QUADS );
					glTexCoord3d(0.0,0.0,.5); glVertex2i(x, y);
					glTexCoord3d(1.0,0.0,.5); glVertex2i(x, y+dY);
					glTexCoord3d(1.0,1.0,.5); glVertex2i(x+dX, y+dY);
					glTexCoord3d(0.0,1.0,.5); glVertex2i(x+dX, y);
				glEnd();
			glDisable( GL_TEXTURE_2D );
		}
	}

	glEnable( GL_TEXTURE_2D );
		// bind the correct tecture based on this unit
		glBindTexture( GL_TEXTURE_2D, 19 );
				
		// draw the unit to the screen
		glColor4f(0.0, 0.0, 0.0, 0.75f);
		glBegin( GL_QUADS );
			glTexCoord3d(0.0,0.0,.5); glVertex2i(0, 0);
			glTexCoord3d(1.0,0.0,.5); glVertex2i(mapPixelWidth, 0);
			glTexCoord3d(1.0,1.0,.5); glVertex2i(mapPixelWidth, mapPixelHeight);
			glTexCoord3d(0.0,1.0,.5); glVertex2i(0, mapPixelHeight);
		glEnd();
	glDisable( GL_TEXTURE_2D );
}

void Display::RenderUnits()
{
	for (IDType u(0); u<state.numNeutralUnits(); ++u)
	{
		const Unit & unit(state.getNeutralUnit(u));

		RenderUnit(unit);
	}

	for (IDType p(0); p<Constants::Num_Players; ++p)
	{
		for (IDType u(0); u<state.prevNumUnits(p); ++u)
		{
			const Unit &			unit(state.getUnit(p, u));
			
			RenderUnit(unit);
		}
	}
}

void Display::RenderUnit(const Unit & unit)
{
	
	static const float3 factionColors[12] = 
	{
		float3(1,0,0),		float3(0,1,0),			float3(0,1,0.5f),	float3(0.5f,0,1),
		float3(1,0.5f,0),	float3(0.5f,0.25f,0),	float3(1,1,1),		float3(1,1,0),
		float3(0,0,0),		float3(0,0,0),			float3(0,0,0),		float3(0,1,1)
	};
	
	glColor4f(1, 1, 1, 1);
	const int healthBoxHeight = 4;

	const Position			pos(unit.currentPosition(state.getTime()));
	const BWAPI::UnitType	type(unit.type());
	const int				tx(textureSizes[type.getID()].x()/2);
	const int				ty(textureSizes[type.getID()].y()/2);
				
	// unit box will be a square due to having square textures
	const int				x0(pos.x() - type.dimensionUp());
	const int				x1(pos.x() + type.dimensionDown());
	const int				y0(pos.y() - type.dimensionUp());
	const int				y1(pos.y() + type.dimensionDown());

	const int				tx0(pos.x() - tx);
	const int				tx1(pos.x() + tx);
	const int				ty0(pos.y() - ty);
	const int				ty1(pos.y() + ty);


	// if the unit can move right now draw its move
	if (unit.previousActionTime() == state.getTime())
	{
		const UnitAction & move = unit.previousAction();

		if (move.type() == UnitActionTypes::MOVE)
		{
			glColor4f(1, 1, 1, 0.75);
			glBegin(GL_LINES);
				glVertex2i(pos.x(), pos.y());
				glVertex2i(unit.pos().x(), unit.pos().y());
			glEnd( );
		}
		else if (move.type() == UnitActionTypes::ATTACK)
		{
			const Unit &	target(state.getUnit(state.getEnemy(unit.player()), move.index()));
			const Position	targetPos(target.currentPosition(state.getTime()));

			glColor4f(factionColors[unit.player()].x, factionColors[unit.player()].y, factionColors[unit.player()].z, 0.75);
			glBegin(GL_LINES);
				glVertex2i(pos.x(), pos.y());
				glVertex2i(targetPos.x(), targetPos.y());
			glEnd( );

			/*glColor4f(1.0, 0.0, 0.0, 0.25);
			glBegin(GL_QUADS);
				glVertex2i(targetPos.x()-type.dimensionUp(),targetPos.y()-type.dimensionUp());
				glVertex2i(targetPos.x()-type.dimensionUp(),targetPos.y()+type.dimensionUp());
				glVertex2i(targetPos.x()+type.dimensionUp(),targetPos.y()+type.dimensionUp());
				glVertex2i(targetPos.x()+type.dimensionUp(),targetPos.y()-type.dimensionUp());
			glEnd();*/
		}
		else if (move.type() == UnitActionTypes::HEAL)
		{
			const Unit &	target(state.getUnit(unit.player(), move.index()));
			const Position	targetPos(target.currentPosition(state.getTime()));

			glColor4f(factionColors[unit.player()].x, factionColors[unit.player()].y, factionColors[unit.player()].z, 0.75);
			glBegin(GL_LINES);
				glVertex2i(pos.x(), pos.y());
				glVertex2i(targetPos.x(), targetPos.y());
			glEnd( );

			/*glColor4f(0.0, 1.0, 0.0, 0.25);
			glBegin(GL_QUADS);
				glVertex2i(targetPos.x()-type.dimensionUp(),targetPos.y()-type.dimensionUp());
				glVertex2i(targetPos.x()-type.dimensionUp(),targetPos.y()+type.dimensionUp());
				glVertex2i(targetPos.x()+type.dimensionUp(),targetPos.y()+type.dimensionUp());
				glVertex2i(targetPos.x()+type.dimensionUp(),targetPos.y()-type.dimensionUp());
			glEnd();*/
		}
	}
			
	// draw the unit's texture
	glEnable( GL_TEXTURE_2D );
		glColor4f(1, 1, 1, 0.75);
		glBindTexture( GL_TEXTURE_2D, unit.type().getID() );
		glBegin( GL_QUADS );
			glTexCoord3d(0.0,0.0,.5); glVertex2i(tx0,ty0);
			glTexCoord3d(1.0,0.0,.5); glVertex2i(tx1,ty0);
			glTexCoord3d(1.0,1.0,.5); glVertex2i(tx1,ty1);
			glTexCoord3d(0.0,1.0,.5); glVertex2i(tx0,ty1);
		glEnd();
	glDisable( GL_TEXTURE_2D );

	if (unit.player() == Players::Player_None)
	{
		return;
	}

	// draw the unit HP box
	double	percHP = (double)unit.currentHP() / (double)unit.maxHP();
	int		cw = (int)((x1-x0) * percHP);
	int		xx = pos.x() - (x1-x0)/2;
	int		yy = pos.y() - healthBoxHeight - (y1-y0)/2 - 5;

	glColor4f(factionColors[unit.player()].x, factionColors[unit.player()].y, factionColors[unit.player()].z, 0.75);
	glBegin(GL_QUADS);
		glVertex2i(xx,yy);
		glVertex2i(xx+cw,yy);
		glVertex2i(xx+cw,yy+healthBoxHeight);
		glVertex2i(xx,yy+healthBoxHeight);
	glEnd();

	// draw the unit energy box
	if (unit.currentEnergy() > 0)
	{
		double	percEnergy = (double)unit.currentEnergy() / (double)Constants::Starting_Energy;
		cw = (int)((x1-x0) * percEnergy);
		xx = pos.x() - (x1-x0)/2;
		yy = pos.y() - healthBoxHeight*2 - (y1-y0)/2 - 5;

		glColor4f(0.0, 1.0, 0.0, 0.75);
		glBegin(GL_QUADS);
			glVertex2i(xx,yy);
			glVertex2i(xx+cw,yy);
			glVertex2i(xx+cw,yy+healthBoxHeight);
			glVertex2i(xx,yy+healthBoxHeight);
		glEnd();
	}

	// draw attack radius
	glColor4f(0.2f, 0.2f, 0.2f, 0.25);
	int radius = unit.canHeal() ? unit.healRange() : unit.range();
	//DrawCircle((float)pos.x(), (float)pos.y(), (float)radius, 60);
}

void Display::DrawText(const int & x, const int & y, const int & size, const std::string & text)
{
    glEnable( GL_TEXTURE_2D );
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		{
			glOrtho(0,windowSizeX,windowSizeY,0,-1,1);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			{
                font.Begin();
                font.TextOut(text, x, y, 0);
			}
			glPopMatrix();
		}
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}
	glPopAttrib();
    glDisable( GL_TEXTURE_2D );
}

void Display::DrawCircle(float cx, float cy, float r, int num_segments) const
{ 
	float theta = 2 * (float)3.1415926 / float(num_segments); 
	float c = cosf(theta);//precalculate the sine and cosine
	float s = sinf(theta);
	float t;

	float x = r;//we start at angle = 0 
	float y = 0; 

	glBegin(GL_LINE_LOOP); 
	for(int ii = 0; ii < num_segments; ii++) 
	{ 
		glVertex2f(x + cx, y + cy);//output vertex 

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	} 
	glEnd(); 
}

float3 Display::GetWalkTileColor(int x, int y) const
{
	return ((x + y) % 2) == 0 ? float3(0.0f, 0.0f, 0.0f) : float3(0.05f, 0.05f, 0.05f);
}
 
void Display::LoadTextures() 
{
	std::stringstream ss;
#ifdef WIN32
	ss << imageDir << "ground\\ground" << 2 << ".png";
#else
    ss << imageDir << "ground/ground" << 2 << ".png";
#endif

	std::vector<GLuint> textures(BWAPI::UnitTypes::allUnitTypes().size());
	glGenTextures( BWAPI::UnitTypes::allUnitTypes().size(), &textures[0] );

	LoadTexture(4, ss.str().c_str());

	BOOST_FOREACH (BWAPI::UnitType type, BWAPI::UnitTypes::allUnitTypes())
	{
        LoadTexture(type.getID(), getTextureFileName(type).c_str());
	}
}

void Display::LoadTexture(int textureNumber, const char * fileName)
{
	struct stat buf;
	if (stat(fileName, &buf) == -1)
	{
		return;
	}

//	GLuint texture;			// This is a handle to our texture object
	SDL_Surface *surface;	// This surface will tell us the details of the image
	GLenum texture_format;
	GLint  nOfColors;

	if ( (surface = IMG_Load(fileName)) ) 
	{ 
		// Check that the image's width is a power of 2
		if ( (surface->w & (surface->w - 1)) != 0 ) 
		{
			//printf("warning: image.bmp's width is not a power of 2\n");
		}

		// Also check if the height is a power of 2
		if ( (surface->h & (surface->h - 1)) != 0 ) 
		{
			//printf("warning: image.bmp's height is not a power of 2\n");
		}

		// get the number of channels in the SDL surface
		nOfColors = surface->format->BytesPerPixel;
		if (nOfColors == 4)     // contains an alpha channel
		{
			//printf("Contains Alpha\n");
			if (surface->format->Rmask == 0x000000ff)
				texture_format = GL_RGBA;
			else
				texture_format = GL_BGRA;
		} 
		else if (nOfColors == 3)     // no alpha channel
		{
			if (surface->format->Rmask == 0x000000ff)
				texture_format = GL_RGB;
			else
				texture_format = GL_BGR;
		} 
		else 
		{
			printf("warning: the image is not truecolor..  this will probably break\n");
			// this error should not go unhandled
		}

		// Have OpenGL generate a texture object handle for us
		

		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, textureNumber );

		// Set the texture's stretching properties
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		// Edit the texture object's image data using the information SDL_Surface gives us
		glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels );

		textureSizes[textureNumber] = Position(surface->w, surface->h);
	} 
	else 
	{
		printf("SDL could not load image: %s\n", SDL_GetError());
		//SDL_Quit();
	}    

	// Free the SDL_Surface only if it was successfully created
	if ( surface ) 
	{ 
		//printf("Loaded Image %s\n", fileName);
		SDL_FreeSurface( surface );
	}
}

void Display::LoadMapTexture(SparCraft::Map * map, int textureNumber)
{

	if (!map)
	{
		return;
	}

	unsigned int * data = map->getRGBATexture();

	glBindTexture( GL_TEXTURE_2D, textureNumber );

	// Set the texture's stretching properties
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	// Edit the texture object's image data using the information SDL_Surface gives us
	glTexImage2D( GL_TEXTURE_2D, 0, 4, map->getWalkTileWidth(), map->getWalkTileHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, data );

	delete [] data;
}

void Shape::OnRender() const
{
	glColor3fv(color);
	if(points.size() == 1)
	{
		glBegin(GL_POINTS);
		glVertex2iv(points[0]);
		glEnd();
	}
	else if(points.size() == 2)
	{
		glBegin(GL_LINES);
		glVertex2iv(points[0]);
		glVertex2iv(points[1]);
		glEnd();
	}
	else
	{
		glBegin(solid ? GL_TRIANGLE_FAN : GL_LINE_STRIP);

		BOOST_FOREACH (const int2 & point, points)
		{
			glVertex2iv(point);
		}

		// If not solid, close the outline
		if(!solid)
		{
			glVertex2iv(points[0]);
		}

		glEnd();
	}
}

const std::string Display::getTextureFileName(const BWAPI::UnitType type) const
{
    std::stringstream image;
#ifdef WIN32
    image << imageDir << "units\\" << type.getName() << ".png";
#else
    image << imageDir << "units/" << type.getName() << ".png";
#endif
	std::string filename = image.str();

	for (size_t i(0); i<filename.size(); ++i)
	{
		if (filename[i] == ' ')
		{
			filename[i] = '_';
		}
	}

	return filename;
}

/*void Display::RenderTextOverlay()
{
	static const float texCharWidth(8.0f/256);
	static const float texCharHeight(14.0f/256);

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		{
			glOrtho(0,windowSizeX,windowSizeY,0,-1,1);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,texFont);

			glBegin(GL_QUADS);
			BOOST_FOREACH (const TextElement & element, textElements)
			{
				glColor3fv(element.color);
				int x(element.position.x);
				BOOST_FOREACH(char ch, element.text)
				{
					const float s0((ch % 32) * texCharWidth),  s1(s0 + texCharWidth);
					const float t0((ch / 32) * texCharHeight), t1(t0 + texCharHeight);

					const int x0(x), x1(x0 + 8);
					const int y0(element.position.y), y1(y0 + 14);
					x = x1;

					glTexCoord2f(s0,t0); glVertex2i(x0,y0);
					glTexCoord2f(s1,t0); glVertex2i(x1,y0);
					glTexCoord2f(s1,t1); glVertex2i(x1,y1);
					glTexCoord2f(s0,t1); glVertex2i(x0,y1);
				}
			}
			glEnd();
		}
		glPopMatrix();
	}
	glPopAttrib();
}*/

#endif