#include "GUI.h"
#include "GUITools.h"

using namespace SparCraft;

GLfloat white[4] = {1.0f, 1.0f, 1.0f, 1.0f};

const int GUI::TextureFont = 198;

GUI::GUI(const int mw, const int mh) 
	: windowSizeX(1280)
	, windowSizeY(720)
	, mapWidth(mw)
	, mapHeight(mh)
	, bl(false)
	, br(false)
	, bu(false)
	, bd(false)
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

GUI::~GUI()
{
	SDL_Quit();
}

void GUI::OnStart()
{
	if (started)
	{
		return;
	}

	mapPixelWidth	= mapWidth * 32;
	mapPixelHeight	= mapHeight * 32;

    // start with the origin in the middle of the viewport
	cameraX			= windowSizeX / 2;
	cameraY			= windowSizeY / 2;
    
    //(*wglGetProcAddress("wglSwapIntervalEXT"));

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_GL_SetSwapInterval(1);
	
    window = SDL_CreateWindow("Prismata OpenGL Visualization",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          windowSizeX, windowSizeY,
                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    glcontext = SDL_GL_CreateContext(window);

    LoadTextures();

	// enable alpha blending for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );
	
	// load the unit textures for later use
	LoadTextures();

	started = true;
}

void GUI::SetImageDir(const std::string & filename)
{
    imageDir = filename;
}

void GUI::SetState(const GameState & s)
{
	state = s;
}

void GUI::SetResults(const IDType & player, const std::vector<std::vector<std::string> > & r)
{
	results[player] = r;
}

void GUI::SetParams(const IDType & player, const std::vector<std::vector<std::string> > & p)
{
	params[player] = p;
}

void GUI::SetExpDesc(const std::vector<std::vector<std::string> > & d)
{
	exp = d;
}

void GUI::SetPlayerTypes(const IDType & p1, const IDType & p2)
{
	playerTypes[0] = p1;
	playerTypes[1] = p2;
}

void GUI::OnFrame()
{
	// Handle input events
	HandleEvents();

	// Render the frame
	glClear(GL_COLOR_BUFFER_BIT);
	RenderMainMap();
	RenderTextOverlay();
	
	SDL_GL_SwapWindow(window);
}

void GUI::HandleEvents()
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

void GUI::RenderMainMap()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		{
			glOrtho(0,windowSizeX,windowSizeY,0,-1,1);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			{
				glTranslatef(static_cast<float>(-cameraX),static_cast<float>(-cameraY),0);

				const int vx0(cameraX), vx1(cameraX+windowSizeX);
				const int vy0(cameraY), vy1(cameraY+windowSizeY);

				RenderTerrain(	std::max(vx0>>3,0), std::max(vy0>>3,0), 
					std::min((vx1>>3) + 1, mapWidth*4),
					std::min((vy1>>3) + 1, mapHeight*4));
				RenderUnits();
                RenderInformation();
			}
			

			glPopMatrix();
		}
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}
	glPopAttrib();
}

void GUI::RenderTextOverlay()
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

void GUI::DrawSearchResults(int x, int y)
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
        GUITools::DrawString(Position(x,y), "Player 1 Search Results", white);
        glColor3f(1.0, 1.0, 1.0);

        for (size_t r(0); results[0].size() > 0 && r<results[0][0].size(); ++r)
        {
            GUITools::DrawString(Position(x, y+((r+1)*(size+spacing))), results[0][0][r], white);
            GUITools::DrawString(Position(x+colwidth, y+((r+1)*(size+spacing))), results[0][1][r], white);
        }
    }

    // Player 2 Settings
    if (results[1].size() > 0)
    {
        x += playerspacing;
        glColor3f(0.0, 1.0, 0.0);
        GUITools::DrawString(Position(x, y), "Player 2 Search Results", white);
        glColor3f(1.0, 1.0, 1.0);

        for (size_t r(0); results[1].size() > 0 && r<results[1][0].size(); ++r)
        {
            GUITools::DrawString(Position(x, y+((r+1)*(size+spacing))), results[1][0][r], white);
            GUITools::DrawString(Position(x+colwidth, y+((r+1)*(size+spacing))), results[1][1][r], white);
        }
    }
}

void GUI::DrawParameters(int x, int y)
{
    int size = 11;
    int spacing = 3;
    int colwidth = 175;
    int playerspacing = 350;

    // Player 1 Settings
    if (params[0].size() > 0)
    {
        glColor3f(1.0, 0.0, 0.0);
        GUITools::DrawString(Position(x, y), "Player 1 Settings", white);
        glColor3f(1.0, 1.0, 1.0);

        for (size_t p(0); params[0].size() > 0 && p<params[0][0].size(); ++p)
        {
            GUITools::DrawString(Position(x, y+((p+1)*(size+spacing))), params[0][0][p], white);
            GUITools::DrawString(Position(x+colwidth, y+((p+1)*(size+spacing))), params[0][1][p], white);
        }
    }

    if (params[1].size() > 0)
    {
        // Player 2 Settings
        x += playerspacing;
        glColor3f(0.0, 1.0, 0.0);
        GUITools::DrawString(Position(x, y), "Player 2 Settings", white);
        glColor3f(1.0, 1.0, 1.0);

        for (size_t p(0); params[1].size() > 0 && p<params[1][0].size(); ++p)
        {
            GUITools::DrawString(Position(x, y+((p+1)*(size+spacing))), params[1][0][p], white);
            GUITools::DrawString(Position(x+colwidth, y+((p+1)*(size+spacing))), params[1][1][p], white);
        }
    }
}

void GUI::DrawExperiment(int x, int y)
{
    int size = 11;
    int spacing = 3;
    int colwidth = 100;
    int playerspacing = 350;

    std::stringstream ss;
    ss << "Game Frame: " << state.getTime();

    glColor3f(1.0, 0.0, 0.0);
    GUITools::DrawString(Position(x, y), ss.str(), white);
    glColor3f(1.0, 1.0, 1.0);

    for (size_t p(0); p<exp[0].size(); ++p)
    {
        GUITools::DrawString(Position(x, y+((p+1)*(size+spacing))), exp[0][p], white);
        GUITools::DrawString(Position(x+colwidth, y+((p+1)*(size+spacing))), exp[1][p], white);
    }
}

void GUI::RenderInformation()
{
	static const float3 factionColors[12] = 
	{
		float3(1,0,0),		float3(0,1,0),			float3(0,1,0.5f),	float3(0.5f,0,1),
		float3(1,0.5f,0),	float3(0.5f,0.25f,0),	float3(1,1,1),		float3(1,1,0),
		float3(0,0,0),		float3(0,0,0),			float3(0,0,0),		float3(0,1,1)
	};

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

void GUI::RenderTerrain(int wx0, int wy0, int wx1, int wy1)
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

void GUI::RenderUnits()
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
			const Unit & unit(state.getUnit(p, u));
			
			RenderUnit(unit);
		}
	}
}

void GUI::RenderUnit(const Unit & unit)
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
		const Action & move = unit.previousAction();

		if (move.type() == ActionTypes::MOVE)
		{
			glColor4f(1, 1, 1, 0.75);
			glBegin(GL_LINES);
				glVertex2i(pos.x(), pos.y());
				glVertex2i(unit.pos().x(), unit.pos().y());
			glEnd( );
		}
		else if (move.type() == ActionTypes::ATTACK)
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
		else if (move.type() == ActionTypes::HEAL)
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


float3 GUI::GetWalkTileColor(int x, int y) const
{
	return ((x + y) % 2) == 0 ? float3(0.0f, 0.0f, 0.0f) : float3(0.05f, 0.05f, 0.05f);
}
 
void GUI::LoadTextures() 
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

	for (const BWAPI::UnitType & type : BWAPI::UnitTypes::allUnitTypes())
	{
        LoadTexture(type.getID(), getTextureFileName(type).c_str());
	}
}

void GUI::LoadTexture(int textureNumber, const std::string & fileName)
{
    struct stat buf;
    if (stat(fileName.c_str(), &buf) == -1)
    {
        return;
    }

    SDL_Surface *surface2 = IMG_Load(fileName.c_str()); 
    GLenum texture_format = GL_RGBA;

    if (surface2 != NULL)
    {
        glBindTexture( GL_TEXTURE_2D, textureNumber );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, texture_format, surface2->w, surface2->h, 0, texture_format, GL_UNSIGNED_BYTE, surface2->pixels);
    } 
    else 
    {
        printf("SDL could not load image: %s\n", SDL_GetError());
    }    

    if ( surface2 ) 
    { 
        SDL_FreeSurface( surface2 );
    }
}

void GUI::LoadMapTexture(SparCraft::Map * map, int textureNumber)
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

const std::string GUI::getTextureFileName(const BWAPI::UnitType type) const
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
