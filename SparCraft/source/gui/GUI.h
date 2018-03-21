#pragma once

#include "../SparCraft.h"
#include "../Timer.h"
#include "../Position.hpp"
#include "BWAPI.h"
#include "GUITools.h"
#include "GUIGame.h"
#include <vector>

#include <SDL2/SDL.h>
#undef main

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include "GUITools.h"
#include <sys/stat.h>

namespace SparCraft
{

class GUI
{
    int                 _initialWidth;
    int                 _initialHeight;
    int                 _windowSizeY;
    double                 _cameraX;
    double                 _cameraY;
    bool                _isStarted;
    int                 _previousMouseX;
    int                 _previousMouseY;
    bool                _mousePressed;
    bool                _shiftPressed;
    double              _previousRenderTime;
    double              _zoom;
	size_t				_frameDelayMS;

    size_t              _currentFrame;

    SDL_Window *        _window;
    SDL_Surface *       _surface;
    SDL_GLContext       _glcontext;

    GUIGame             _guiGame;
	std::string			_imagesDir;

    std::vector<GLuint>     _textures;
    std::vector<Position>   _textureSizes;

    std::map<BWAPI::UnitType, int> _unitTypeTextureID;
    std::map<BWAPI::TechType, int> _techTypeTextureID;
    std::map<BWAPI::UpgradeType, int> _upgradeTypeTextureID;
    
    void handleEvents();
    void render();
    void renderTextGlut(int x, int y, std::string & s);
    void loadTextures(std::string imageDir);
    bool loadTexture(int textureNumber, const std::string & fileName);
    void onResize(SDL_Event & event);
    void drawAllBWAPIUnits();
    void onStart();
    void testRender();
    
    bool isStarted() const;

    static std::string GetTextureFileName(const BWAPI::UnitType & type);
    static std::string GetTextureFileName(const BWAPI::UpgradeType & type);
    static std::string GetTextureFileName(const BWAPI::TechType & type);

public:
    
    static const int TextureASCIIOffset;
    static const int TextureFont;
    
    GUI(int width, int height, std::string imagesDir);
    ~GUI();

    int width();
    int height();

    void onFrame();
    void setCenter(int x, int y);
    void setGame(const Game & game);
    void drawUnitType(const BWAPI::UnitType & type, const Position & p);
	void setUpdateDelay(const size_t & delayMS);

    const Game & getGame() const;

    bool saveScreenshotBMP(const std::string & filename);
};
}
