#pragma once

#define NO_SDL_GLEXT

#include "Common.h"
#include "GameState.h"
#include "AlphaBetaSearchResults.hpp"
#include "AlphaBetaSearchParameters.hpp"
#include "UCTSearchParameters.hpp"
#include "Timer.h"

#include <SDL.h>
#undef main

#include <SDL_opengl.h>
#include <SDL_image.h>

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

class GUI
{
	int	                        windowSizeX;
	int                         windowSizeY;
	int                         cameraX;
	int                         cameraY;
	int                         mapWidth;
	int                         mapHeight;
	int                         mapPixelWidth;
	int                         mapPixelHeight;
	bool                        bl,br,bd,bu;

	std::vector<Position>       textureSizes;

    std::string                 imageDir;

    SDL_Window *                window;
	SDL_Surface *               screen;
    SDL_GLContext               glcontext;

	GameState                   state;

	IDType                      playerTypes[2];

	bool                        started;
	bool                        drawResults;

    std::vector<std::vector<std::string> >    params[2];
    std::vector<std::vector<std::string> >    results[2];
    std::vector<std::vector<std::string> >    exp;

	void HandleEvents();

	void RenderMainMap();
	void RenderTextOverlay();
	void RenderInformation();
	
	const std::string getTextureFileName(const BWAPI::UnitType type) const;

	void LoadTextures();
	void LoadTexture(int textureNumber, const std::string & filename);

	void DrawSearchResults(int x, int y);
	void DrawParameters(int x, int y);
    void DrawExperiment(int x, int y);
	
    void DrawTextVector(const int & x, const int & y, const int & size, const std::vector<std::vector<std::string> > & text);

	void DrawUnitTexture(const Unit & unit) const;
	void DrawUnitMove(const Unit & unit, const float3 & color) const;
	void DrawUnitHP(const Unit & unit, const float3 & color) const;

	void RenderTerrain(int wx0, int wy0, int wx1, int wy1);
	void RenderUnits();
	void RenderUnit(const SparCraft::Unit & unit);

	float3 GetWalkTileColor(int x, int y) const;


public:
    
    static const int            TextureFont;

	GUI(const int mw, const int mh);
	~GUI();

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
};
}


/*
#ifdef USING_VISUALIZATION_LIBRARIES
        if (disp)
        {
            GameState copy(state);
            GameState copy2(state);
            copy2.finishedMoving();

            TimeType nextTime = std::min(copy2.getUnit(0, 0).firstTimeFree(), copy2.getUnit(1, 0).firstTimeFree());

            // set the parameter overlays for search players
            for (IDType p(0); p<Constants::Num_Players; ++p)
            {
                Player_UCT *        uct = dynamic_cast<Player_UCT *>        (_players[p].get());
                Player_AlphaBeta *  ab  = dynamic_cast<Player_AlphaBeta *>  (_players[p].get());

                if (uct) 
                { 
                    disp->SetParams(p, uct->getParams().getDescription());
                    disp->SetResults(p, uct->getResults().getDescription());
                }

                if (ab) 
                { 
                    disp->SetParams(p, ab->getParams().getDescription()); 
                    disp->SetResults(p, ab->results().getDescription());
                }
            }
            
            bool everyFrame = true;
            if (everyFrame)
            {
                while (copy.getTime() < nextTime)
                {
                    while (frameTimer.getElapsedTimeInMilliSec() < 30);
                    frameTimer.start();
                    disp->SetState(copy);
                    disp->OnFrame();
                    copy.setTime(copy.getTime()+1);
                }
            }
            else
            { 
                disp->SetState(state);
                disp->OnFrame();
            }
        }
#endif
*/