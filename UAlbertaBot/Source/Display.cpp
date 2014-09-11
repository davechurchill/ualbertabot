#include "Common.h"
#include "Display.h"

#include <BWAPI.h>
#include <BWTA.h>
using namespace BWAPI;

#ifdef USE_SDL

#include <SDL.h>
#include <SDL_opengl.h>

Display::Display() : windowSizeX(640), windowSizeY(480), bl(false), br(false), bu(false), bd(false)
{
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
	mapWidth		= Broodwar->mapWidth();
	mapHeight		= Broodwar->mapHeight();

	mapPixelWidth	= mapWidth * 32;
	mapPixelHeight	= mapHeight * 32;

	const BWAPI::Position position(Broodwar->self()->getStartLocation());
	cameraX			= position.x() - windowSizeX / 2;
	cameraY			= position.y() - windowSizeY / 2;

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_SetVideoMode(windowSizeX, windowSizeY, 32, SDL_OPENGL);
}

void Display::OnFrame()
{
	// Handle input events
	HandleEvents();

	// Render the frame
	glClear(GL_COLOR_BUFFER_BIT);
	RenderMainMap();
	RenderMinimap();
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
		case SDL_QUIT:
			throw std::runtime_error("The user is a quitter");
			break;
		}
	}

	// Move the camera
	const int cameraSpeed(128);
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

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glOrtho(0,windowSizeX,windowSizeY,0,-1,1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(static_cast<float>(-cameraX),static_cast<float>(-cameraY),0);

	const int vx0(cameraX), vx1(cameraX+windowSizeX);
	const int vy0(cameraY), vy1(cameraY+windowSizeY);

	RenderTerrain(	std::max(vx0>>3,0), std::max(vy0>>3,0), 
					std::min((vx1>>3) + 1, mapWidth*4),
					std::min((vy1>>3) + 1, mapHeight*4));
	RenderTerrainAnalysis();
	RenderUnits();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

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
	RenderUnits();

	// Render outline around viewport on minimap
	{
		const int vpx0(cameraX), vpx1(vpx0 + windowSizeX);
		const int vpy0(cameraY), vpy1(vpy0 + windowSizeY);

		glBegin(GL_LINE_STRIP);
		glColor3f(1,1,1);
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

void Display::RenderTerrain(int wx0, int wy0, int wx1, int wy1) const
{
	glBegin(GL_QUADS);
	for(int y(wy0); y<wy1; ++y)
	{
		for(int x(wx0); x<wx1; ++x)
		{
			// Determine coordinates of walk tile
			const int x0(x * 8), x1(x0 + 8);
			const int y0(y * 8), y1(y0 + 8);

			glColor3fv(GetWalkTileColor(x,y));
			glVertex2i(x0,y0);
			glVertex2i(x1,y0);
			glVertex2i(x1,y1);
			glVertex2i(x0,y1);
		}
	}
	glEnd();
}

void Display::RenderTerrainAnalysis() const
{
	glBegin(GL_LINES);

	// Draw polygon outlines of regions in green
	glColor3f(0,1,0);
	BOOST_FOREACH(BWTA::Region * region, BWTA::getRegions())
	{
		const BWTA::Polygon & polygon(region->getPolygon());
		for(size_t i(0); i<polygon.size(); ++i)
		{
			const Position & v0(polygon[i]);
			const Position & v1(polygon[(i+1) % polygon.size()]);
			glVertex2i(v0.x(), v0.y());
			glVertex2i(v1.x(), v1.y());
		}
	}

	// Draw chokepoints in red
	glColor3f(1,0,0);
	BOOST_FOREACH(BWTA::Region * region, BWTA::getRegions())
	{
		BOOST_FOREACH(BWTA::Chokepoint * choke, region->getChokepoints())
		{
			const Position & v0(choke->getSides().first);
			const Position & v1(choke->getSides().second);
			glVertex2i(v0.x(), v0.y());
			glVertex2i(v1.x(), v1.y());
		}
	}

	glEnd();
}

void Display::RenderUnits() const
{
	static const float3 factionColors[12] = 
	{
		float3(1,0,0),		float3(0,0,1),			float3(0,1,0.5f),	float3(0.5f,0,1),
		float3(1,0.5f,0),	float3(0.5f,0.25f,0),	float3(1,1,1),		float3(1,1,0),
		float3(0,0,0),		float3(0,0,0),			float3(0,0,0),		float3(0,1,1)
	};

	glBegin(GL_QUADS);
	BOOST_FOREACH(Unit * unit, Broodwar->getAllUnits())
	{
		const UnitType type(unit->getType());
		const int x0(unit->getPosition().x() - type.dimensionLeft());
		const int x1(unit->getPosition().x() + type.dimensionRight());
		const int y0(unit->getPosition().y() - type.dimensionUp());
		const int y1(unit->getPosition().y() + type.dimensionDown());

		glColor3fv(factionColors[unit->getPlayer()->getID()]);
		glVertex2i(x0,y0);
		glVertex2i(x1,y0);
		glVertex2i(x1,y1);
		glVertex2i(x0,y1);
	}
	glEnd();
}

float3 Display::GetWalkTileColor(int x, int y) const
{
	// Unexplored areas return black
	const TilePosition pos(x/4,y/4);
	if(!Broodwar->isExplored(pos))
	{
		return 0;
	}

	// Basic terrain color
	float3 color(0.2f);
	if(Broodwar->isWalkable(x,y))
	{
		color = 0.4f + Broodwar->getGroundHeight(x,y) * 0.2f;
		if(!Broodwar->isBuildable(pos))
		{
			color *= float3(1,0.5f,0.5f);
		}
	}

	// Adjust for creep
	if(Broodwar->hasCreep(pos))
	{
		color = float3(0.2f,0,0.3f);
	}

	// Adjust for fog of war
	if(!Broodwar->isVisible(pos))
	{
		color /= 2;
	}
	return color;
}

#else

Display::Display() {}
Display::~Display() {}
void Display::OnStart() {}
void Display::OnFrame() {}

#endif