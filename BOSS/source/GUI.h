#pragma once

#include "Common.h"
#include "BOSS.h"
#include "GUITools.h"

#include <SDL.h>
#undef main

#include <SDL_opengl.h>
#include <SDL_image.h>
#include "GameState.h"
#include "GUITools.h"
#include <sys/stat.h>
#include "BOSSVisExperiment.h"

namespace BOSS
{

class BOSSVisExperiment;

class GUI
{
    unsigned int                texFont;
    int                         windowSizeX;
    int                         windowSizeY;
    int                         cameraX;
    int                         cameraY;
    int                         mapPixelWidth;
    int                         mapPixelHeight;
    bool                        bl,br,bd,bu;
    bool                        started;
    int                         previousMouseX;
    int                         previousMouseY;
    bool                        mousePressed;

    bool                        shiftPressed;

    double                      zoomX, zoomY;

    size_t                      currentFrame;
    Timer                       timer;
    double                      lastFrameTime;

    SDL_Window *                window;
    SDL_Surface *               screen;
    SDL_GLContext               glcontext;

    std::vector<GLuint>         textures;
    std::vector<Position>       textureSizes;

    BOSSVisExperiment           _currentExperiment;
    
    void HandleEvents();
    void Render();
    void RenderTextGlut(int x, int y, std::string & s);
    void LoadTextures();
    void LoadTexture(int textureNumber, const std::string & fileName);
    void OnResize(SDL_Event & event);
    void DrawAllUnits();
    void DrawGameState();
    PositionType DrawVisExperiment(const Position & pos, const GameState & currentState, const std::vector<ActionType> & buildOrder, const size_t & boIndex, const std::vector<FrameCountType> & startTimes, const std::vector<FrameCountType> & finishTimes);
    

    std::string getTimeString(const FrameCountType & frameCount);

    const std::string getTextureFileName(const ActionType & type) const;
    const int getTextureNumber(const ActionType & type) const;

    PositionType DrawScenario(const Position & pos, const size_t scenario);

    GUI();

public:
    
    static const int            TextureASCIIOffset;
    static const int            TextureFont;

    static GUI & Instance();
    ~GUI();

    int Width();
    int Height();
    void OnStart();
    void OnFrame();
    bool isStarted() const;
    bool saveScreenshotBMP(const std::string & filename);

    void SetVisExperiment(BOSSVisExperiment & exp);

    void DrawActionType(const ActionType & type, const Position & tl, const size_t & width);
};
}
