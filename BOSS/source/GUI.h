#pragma once

#include "Common.h"

#ifdef BOSS_ENABLE_GUI

#include "freeglut.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>
#include "GameState.h"
#include "GUITools.h"
#include <sys/stat.h>
#include "glut.h"

namespace BOSS
{

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

    SDL_Surface *               screen;

    std::vector<GLuint>         textures;
    std::vector<Position>       textureSizes;

    std::vector<GameState>                 _currentStates;
    std::vector<std::vector<ActionType> >  _buildOrders;
    std::vector<size_t>                      _boIndexes;
    std::vector<std::vector<FrameCountType> > _startTimes;
    std::vector<std::vector<FrameCountType> > _finishTimes;

#ifdef WIN32
    void setVSync(int interval);
#endif

    void HandleEvents();
    void Render();
    void RenderTextGlut(int x, int y, std::string & s);
    void LoadTextures();
    void LoadTexture(int textureNumber, const char * fileName, bool mipmap = false);
    void OnResize(SDL_Event & event);
    void DrawAllUnits();
    void DrawGameState();
    PositionType DrawConcurrency(const Position & pos, const GameState & currentState, const std::vector<ActionType> & buildOrder, const size_t & boIndex, const std::vector<FrameCountType> & startTimes, const std::vector<FrameCountType> & finishTimes);
    void DrawActionType(const ActionType & type, const Position & tl, const size_t & width);

    std::string getTimeString(const FrameCountType & frameCount);

    const std::string getTextureFileName(const ActionType & type) const;
    const int getTextureNumber(const ActionType & type) const;

    GUI();

public:

    static GUI & Instance();
    ~GUI();

    void OnStart();
    void OnFrame();
    bool isStarted() const;
    bool saveScreenshotBMP(const std::string & filename);

    void AddState(const GameState & state);
    void AddBuildOrder(const std::vector<ActionType> & buildOrder, const size_t boIndex);
    void AddActionTimes(const std::vector<FrameCountType> & startTimes, std::vector<FrameCountType> & endTimes);
};
}

#endif