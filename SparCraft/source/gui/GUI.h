#pragma once

#include "../SparCraft.h"
#include "../Timer.h"
#include "../Position.hpp"
#include "BWAPI.h"
#include "GUITools.h"
#include "GUIGame.h"
#include <vector>

#include <SDL.h>
#undef main

#include <SDL_opengl.h>
#include <SDL_image.h>
#include "GUITools.h"
#include <sys/stat.h>

namespace SparCraft
{

class GUI
{
    int                 _initialWidth;
    int                 _initialHeight;
    int                 _windowSizeY;
    int                 _cameraX;
    int                 _cameraY;
    bool                _isStarted;
    int                 _previousMouseX;
    int                 _previousMouseY;
    bool                _mousePressed;
    bool                _shiftPressed;
    double              _previousRenderTime;

    size_t              _currentFrame;

    SDL_Window *        _window;
    SDL_Surface *       _surface;
    SDL_GLContext       _glcontext;

    GUIGame             _guiGame;

    std::vector<GLuint>     _textures;
    std::vector<Position>   _textureSizes;

    std::map<BWAPI::UnitType, int> _unitTypeTextureID;
    std::map<BWAPI::TechType, int> _techTypeTextureID;
    std::map<BWAPI::UpgradeType, int> _upgradeTypeTextureID;
    
    void handleEvents();
    void render();
    void renderTextGlut(int x, int y, std::string & s);
    void loadTextures();
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
    
    GUI(int width, int height);
    ~GUI();

    int width();
    int height();

    void onFrame();
    void setCenter(int x, int y);
    void setGame(const Game & game);
    void drawUnitType(const BWAPI::UnitType & type, const Position & p);

    const Game & getGame() const;

    bool saveScreenshotBMP(const std::string & filename);
};
}
