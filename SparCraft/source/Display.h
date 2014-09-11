#include "Common.h"

#ifdef USING_VISUALIZATION_LIBRARIES

#pragma once

#include "GameState.h"
#include "AlphaBetaSearchResults.hpp"
#include "AlphaBetaSearchParameters.hpp"
#include "UCTSearchParameters.hpp"
#include "Timer.h"

#include "boost/foreach.hpp"

#include "glfont/glfont.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>

#include <sys/stat.h>

namespace SparCraft
{
struct float3
{
	float x,y,z;

	float3() {}
	float3(float f) : x(f), y(f), z(f) {}
	float3(float x, float y, float z) : x(x), y(y), z(z) {}

	operator const float * () const { return &x; }
	float3 operator + (const float3 & v) const { return float3(x+v.x,y+v.y,z+v.z); }
	float3 operator - (const float3 & v) const { return float3(x-v.x,y-v.y,z-v.z); }
	float3 operator * (const float3 & v) const { return float3(x*v.x,y*v.y,z*v.z); }
	float3 operator / (const float3 & v) const { return float3(x/v.x,y/v.y,z/v.z); }

	operator float * () { return &x; }
	const float3 & operator += (const float3 & v) { x+=v.x; y+=v.y; z+=v.z; return *this; }
	const float3 & operator -= (const float3 & v) { x-=v.x; y-=v.y; z-=v.z; return *this; }
	const float3 & operator *= (const float3 & v) { x*=v.x; y*=v.y; z*=v.z; return *this; }
	const float3 & operator /= (const float3 & v) { x/=v.x; y/=v.y; z/=v.z; return *this; }
};

struct int2
{
	int x,y;

	int2() {}
	int2(int x, int y) : x(x), y(y) {}

	operator const int * () const { return &x; }
};

struct Shape
{
	std::vector<int2>	points;
	float3				color;
	bool				solid;
public:
	Shape(const float3 & color, bool solid) : color(color), solid(solid) {}

	void AddPoint(const int2 & point) { points.push_back(point); }

	void OnRender() const;
};

struct TextElement
{
	int2		position;
	float3		color;
	std::string	text;

	TextElement() {}
	TextElement(const int2 & position, const float3 & color, const char * text) : position(position), color(color), text(text) {}
};

class Display
{
	unsigned int                texFont;
	int	                        windowSizeX;
	int                         windowSizeY;
	int                         zoomX;
	int                         zoomY;
	int                         cameraX;
	int                         cameraY;
	int                         mapWidth;
	int                         mapHeight;
	int                         mapPixelWidth;
	int                         mapPixelHeight;
	bool                        bl,br,bd,bu;

	std::vector<Position>       textureSizes;
	std::vector<Shape>          shapes;
	std::vector<TextElement>    textElements;

    std::string                 imageDir;

	SDL_Surface *               screen;

	GameState                   state;

	IDType                      playerTypes[2];

	bool                        started;
	bool                        drawResults;

    std::vector<std::vector<std::string> >    params[2];
    std::vector<std::vector<std::string> >    results[2];
    std::vector<std::vector<std::string> >    exp;

#ifdef WIN32
    void setVSync(int interval);
#endif

	void HandleEvents();

	void RenderMainMap();
	void RenderShapes();
	void RenderMinimap();
	void RenderTextOverlay();
	void RenderInformation();
	
	const std::string getTextureFileName(const BWAPI::UnitType type) const;

	void LoadTextures();
	void LoadTexture(int textureNumber, const char * fileName);

	void DrawSearchResults(int x, int y);
	void DrawParameters(int x, int y);
    void DrawExperiment(int x, int y);
	
	void DrawCircle(float cx, float cy, float r, int num_segments) const;
	void DrawText(const int & x, const int & y, const int & size, const std::string & text);
    void DrawTextVector(const int & x, const int & y, const int & size, const std::vector<std::vector<std::string> > & text);

	void DrawUnitTexture(const Unit & unit) const;
	void DrawUnitMove(const Unit & unit, const float3 & color) const;
	void DrawUnitHP(const Unit & unit, const float3 & color) const;

	void RenderTerrain(int wx0, int wy0, int wx1, int wy1);
	void RenderUnits();
	void RenderUnit(const SparCraft::Unit & unit);

	float3 GetWalkTileColor(int x, int y) const;


public:

	Display(const int mw, const int mh);
	~Display();

	void OnStart();
	void OnFrame();

    void RenderTextGlut(int x, int y, std::string & s);
	void LoadMapTexture(SparCraft::Map * map, int textureNumber);
	void SetState(const GameState & s);
	void SetParams(const IDType & player, const std::vector<std::vector<std::string> > & p);
    void SetResults(const IDType & player, const std::vector<std::vector<std::string> > & r);
    void SetExpDesc(const std::vector<std::vector<std::string> > & r);
	void SetPlayerTypes(const IDType & p1, const IDType & p2);
    
    void SetImageDir(const std::string & filename);

	void AddShape(const Shape & shape) { shapes.push_back(shape); }
	void AddTextElement(const TextElement & element) { textElements.push_back(element); }
};
}

#endif