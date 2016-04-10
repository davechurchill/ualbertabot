#pragma once

#include "BWAPI.h"
#include "GUITools.h"
#include "Timer.hpp"
#include "Position.hpp"
#include <vector>

#include <SDL.h>
#undef main

#include <SDL_opengl.h>
#include <SDL_image.h>
#include "GUITools.h"
#include <sys/stat.h>

using BOSS::Position;

struct TextureInfo
{
    int _id;
    Position _size;

    TextureInfo(int i, Position & s) : _id(i), _size(s)
    {
        
    }
};

class StarCraftGUI
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

    size_t              _currentFrame;

    SDL_Window *        _window;
    SDL_Surface *       _surface;
    SDL_GLContext       _glcontext;

    std::vector<GLuint>     _textures;
    std::vector<Position>   _textureSizes;

    std::map<std::string, TextureInfo> _textureInfo;

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
    void drawUnitType(const BWAPI::UnitType & type, Position & p);
    bool isStarted() const;

    static std::string GetTextureFileName(const BWAPI::UnitType & type);
    static std::string GetTextureFileName(const BWAPI::UpgradeType & type);
    static std::string GetTextureFileName(const BWAPI::TechType & type);

public:
    
    static const int TextureASCIIOffset;
    static const int TextureFont;
    
    StarCraftGUI(int width, int height);
    ~StarCraftGUI();

    int width();
    int height();

    void onFrame();
    void setCenter(int x, int y);

    bool saveScreenshotBMP(const std::string & filename);
};

