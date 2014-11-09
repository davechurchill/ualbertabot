#include "GUI.h"

using namespace BOSS;

#define GUI_INITIAL_WIDTH  1920
#define GUI_INITIAL_HEIGHT 1080

#define BOSS_MAX_TEXTURES 200
#define BOSS_TEXTURE_INTERVAL 64

GUI::GUI() 
    : windowSizeX(GUI_INITIAL_WIDTH)
    , windowSizeY(GUI_INITIAL_HEIGHT)
    , bl(false)
    , br(false)
    , bu(false)
    , bd(false)
    , cameraX(0)
    , cameraY(0)
    , previousMouseX(0)
    , previousMouseY(0)
    , started(false)
    , mousePressed(false)
    , shiftPressed(false)
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        BOSS_ASSERT(false, "Unable to initialise SDL");
    }
}

GUI & GUI::Instance()
{
    static GUI g;
    return g;
}

GUI::~GUI()
{
    SDL_Quit();
}

bool GUI::isStarted() const
{
    return started;
}

void GUI::OnStart()
{
    if (started)
    {
        return;
    }

    cameraX   = 0;
    cameraY   = 0;

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL,1);
    screen = SDL_SetVideoMode(windowSizeX,windowSizeY,32,SDL_OPENGL | SDL_RESIZABLE);
    SDL_WM_SetCaption("BOSS OpenGL Visualization",0);

    LoadTextures();

    // enable alpha blending for transparency
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glViewport(0,0,windowSizeX,windowSizeY);

#ifdef WIN32
    setVSync(0);
#endif

    started = true;
}


void GUI::OnFrame()
{
    BOSS_ASSERT(isStarted(), "Must initialize GUI before calling OnFrame()");

    // Handle input events
    HandleEvents();

    // Render the frame
    glClear(GL_COLOR_BUFFER_BIT);
    Render();

    SDL_GL_SwapBuffers();
}

void GUI::OnResize(SDL_Event & event)
{
    windowSizeX = event.resize.w;
    windowSizeY = event.resize.h;

    glViewport(0,0,windowSizeX,windowSizeY);
    OnFrame();
}

void GUI::HandleEvents()
{
    // Handle SDL events
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        const bool pressed(event.key.state == SDL_PRESSED);
        switch (event.type)
        {
            case SDL_VIDEORESIZE:
            {
                OnResize(event);
                break;
            }
            case SDL_MOUSEMOTION:
            {
                if ((previousMouseX != 0 || previousMouseY != 0) && (event.motion.state & SDL_BUTTON_LEFT))
                {
                    //cameraX -= event.motion.xrel;
                    //cameraY -= event.motion.yrel;
                }
                previousMouseX = event.motion.x;
                previousMouseY = event.motion.y;
                break;
            }
            case SDL_KEYDOWN:
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_LSHIFT:
                    shiftPressed = pressed;
                    break;
                case SDLK_p:
                {
                    static int screenshots = 0;
                    std::stringstream ss;
                    ss << "screenshot_" << screenshots++ << ".bmp";
                    std::cout << "Taking screenshot: " << ss.str() << "\n";
                    saveScreenshotBMP(ss.str());
                    break;
                }
                }
                break;
            }
            case SDL_KEYUP:
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_LSHIFT:
                    shiftPressed = pressed;
                    break;
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                if (event.button.button == SDL_BUTTON_WHEELUP)
                {
                    cameraY -= 40;
                }
                else if (event.button.button == SDL_BUTTON_WHEELDOWN)
                {
                    cameraY += 40;
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    //currentState.addCard((event.button.y > (windowSizeY / 2)) ? 1 : 0, CardType(rand() % CardTypes::GetAllCardTypes().size()));
                }
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {

                }
                break;
            }
            case SDL_QUIT:
            {
                std::cerr << "SDL_QUIT caught\n\n";
                exit(0);
                //MCDS::System::FatalError("Visualization Shut Down");
            }
        }
    }
}

void GUI::Render()
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
                
                Position drawAt(0,0);
                for (size_t i(0); i < _currentStates.size(); ++i)
                {
                    PositionType endY = DrawConcurrency(drawAt, _currentStates[i], _buildOrders[i], _boIndexes[i], _startTimes[i], _finishTimes[i]);
                    drawAt = Position(0, endY);
                }

                _currentStates.clear();
                _buildOrders.clear();
                _boIndexes.clear();
                _startTimes.clear();
                _finishTimes.clear();
            }

            glPopMatrix();
        }
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
    }
    glPopAttrib();
}

std::string GUI::getTimeString(const FrameCountType & frameCount)
{
    std::stringstream min;
    min << (frameCount/24)/60;
    std::string minString = min.str();

    std::stringstream sec;
    sec << (frameCount/24)%60;
    std::string secString = sec.str();
    while (secString.length() < 2) secString = "0" + secString;

    std::string timeString = minString + ":" + secString;
    return timeString;
}

PositionType GUI::DrawConcurrency(const Position & pos, const GameState & currentState, const std::vector<ActionType> & buildOrder, const size_t & boIndex, const std::vector<FrameCountType> & startTimes, const std::vector<FrameCountType> & finishTimes)
{
    
    const std::vector<ActionType> & allActions = ActionTypes::GetAllActionTypes(currentState.getRace());
    static const ActionType & Larva = ActionTypes::GetActionType("Zerg_Larva");
    static const ActionType & Hatchery = ActionTypes::GetActionType("Zerg_Hatchery");
    GLfloat black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat white2[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat white[4] = {1.0f, 1.0f, 1.0f, 0.8f};
    GLfloat grey[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat grey2[4] = {0.4f, 0.4f, 0.4f, 1.0f};
    GLfloat red[4] = {0.6f, 0.0f, 0.0f, 1.0f};
    GLfloat blue[4] = {0.0f, 0.0f, 0.5f, 1.0f};
    GLfloat bblue[4] = {0.5f, 0.5f, 1.0f, 1.0f};
    GLfloat bred[4] = {1.0f, 0.0f, 0.0f, 1.0f};
    GLfloat green[4] = {0.0f, 0.4f, 0.0f, 1.0f};
    GLfloat bgreen[4] = {0.0f, 1.0f, 0.0f, 1.0f};

    std::string stateTime = getTimeString(currentState.getCurrentFrame());

    std::stringstream ssres;
    ssres << "Frame:    " << currentState.getCurrentFrame() <<"\n";
    ssres << "Minerals: " << currentState.getMinerals()/Constants::RESOURCE_SCALE << "\n";
    ssres << "Gas:      " << currentState.getGas()/Constants::RESOURCE_SCALE << "\n";
    ssres << "MWorkers: " << currentState.getNumMineralWorkers() << "\n";
    ssres << "GWorkers: " << currentState.getNumGasWorkers() << "\n";
    ssres << "C Supply: " << currentState.getUnitData().getCurrentSupply() << "\n";
    ssres << "M Supply: " << currentState.getUnitData().getMaxSupply() << "\n";
    GUITools::DrawStringLargeWithShadow(pos + Position(20, 20), ssres.str(), white);

    Position completed = pos + Position(225,0);
    size_t cwidth = 64;
    if (currentState.getRace() == Races::Zerg)
    {
        const UnitCountType & numLarva = currentState.getHatcheryData().numLarva();

        DrawActionType(Larva, completed, cwidth);
        std::stringstream num; num << numLarva << "\n" << (Constants::ZERG_LARVA_TIMER-(currentState.getCurrentFrame() % Constants::ZERG_LARVA_TIMER));
        GUITools::DrawString(completed + Position(10, 20), num.str(), white);
        completed.add(cwidth, 0);
    }

    for (size_t a(0); a < allActions.size(); ++a)
    {
        const size_t numCompleted = currentState.getUnitData().getNumCompleted(allActions[a]);

        if (numCompleted > 0)
        {
            DrawActionType(allActions[a], completed, cwidth);
            std::stringstream num; num << numCompleted;
            GUITools::DrawStringLargeWithShadow(completed + Position(10, 20), num.str(), white);
            completed.add(cwidth, 0);
        }
    }

    Position legal = pos + Position(225,80);
    ActionSet legalActions;
    currentState.getAllLegalActions(legalActions);
    GUITools::DrawStringWithShadow(legal + Position(0,-5), "Legal Actions", white);
    for (size_t a(0); a < legalActions.size(); ++a)
    {
        const ActionType & action = legalActions[a];
        DrawActionType(legalActions[a], legal, 32);
        legal.add(32, 0);
    }


    std::vector<int> layers(startTimes.size(), -1);
    int maxLayer = 0;

    for (size_t i(0); i < startTimes.size(); ++i)
    {
        FrameCountType start    = startTimes[i];
        FrameCountType finish   = finishTimes[i];

        std::vector<int> layerOverlap;
        // loop through everything up to this action and see which layers it can't be in
        for (size_t j(0); j < i; ++j)
        {
            if (start < finishTimes[j])
            {
                layerOverlap.push_back(layers[j]);
            }
        }

        // find a layer we can assign to this value
        int layerTest = 0;
        while (true)
        {
            if (std::find(layerOverlap.begin(), layerOverlap.end(), layerTest) == layerOverlap.end())
            {
                layers[i] = layerTest;
                if (layerTest > maxLayer)
                {
                    maxLayer = layerTest;
                }
                break;
            }

            layerTest++;
        }
    }

    
    float maxWidth = (float)windowSizeX - 430;
    float maxFinishTime = 1;

    for (size_t i(0); i < _finishTimes.size(); ++i)
    {
        for (size_t j(0); j < _finishTimes[i].size(); ++j)
        {
            if (_finishTimes[i][j] > maxFinishTime)
            {
                maxFinishTime = (float)_finishTimes[i][j];
            }
        }
    }

    if (currentState.getCurrentFrame() < maxWidth && currentState.getCurrentFrame() > maxFinishTime)
    {
        maxFinishTime = (float)currentState.getCurrentFrame();
    }

    maxFinishTime = std::max(maxFinishTime, maxWidth);
    //maxFinishTime = std::max(currentState.getCurrentFrame()*1.10f, maxWidth);

    float scale = maxWidth / maxFinishTime;

    // draw it

    PositionType height         = 20;
    PositionType heightBuffer   = 3;
    Position concurrent         = pos + Position(25, 200);

    float framePos              =  (concurrent.x() + scale*currentState.getCurrentFrame());
    Position boxTopLeft         = Position(concurrent.x() - 10, concurrent.y() - 10);
    Position boxBottomRight     = Position(PositionType(concurrent.x() + maxWidth + 10), (PositionType)(concurrent.y() + (maxLayer)*(height + heightBuffer) + 10 + height));
    
    GUITools::DrawRect(boxTopLeft, boxBottomRight, grey2);
    GUITools::DrawRect(boxTopLeft + Position(1,1), boxBottomRight - Position(1,1), grey);

    for (size_t i(0); i < layers.size(); ++i)
    {
        float boxWidth = (finishTimes[i] - startTimes[i])*scale;

        Position topLeft(PositionType(concurrent.x() + startTimes[i]*scale), PositionType(concurrent.y() + (height + heightBuffer) * layers[i]));
        Position bottomRight(PositionType(topLeft.x() + boxWidth), topLeft.y() + height);

        std::string name = buildOrder[i].getName();
        size_t loc = name.find(" ");
        if (loc != std::string::npos)
        {
            name = name.substr(loc);
        }

        GUITools::DrawRect(topLeft, bottomRight, white);
        GUITools::DrawRect(topLeft+Position(1,1), bottomRight-Position(1,1), finishTimes[i] < currentState.getCurrentFrame() ? red : blue);
        GUITools::DrawStringWithShadow(topLeft + Position(2, 13), name, white);
    }

    PositionType boWidth = std::min(32, (PositionType)(maxWidth / buildOrder.size()));
    for (size_t i(0); i < buildOrder.size(); ++i)
    {
        Position topLeft(concurrent.x() + i*boWidth, concurrent.y() - boWidth - 20);

        if (i < boIndex)
        {
            GUITools::DrawRect(topLeft, topLeft + Position(boWidth, boWidth), grey2);
            GUITools::DrawRect(topLeft + Position(1,1), topLeft + Position(boWidth-1, boWidth-1), finishTimes[i] < currentState.getCurrentFrame() ? grey : blue);
        }

        if (i < startTimes.size())
        {
            //GUITools::DrawString(topLeft - Position(-2,20), getTimeString(startTimes[i]).c_str(), bblue);
            //GUITools::DrawString(topLeft - Position(-2,9), getTimeString(finishTimes[i]).c_str(), bred);
        }

        DrawActionType(buildOrder[i], topLeft, boWidth);

        std::string name = buildOrder[i].getName();
        size_t loc = name.find(" ");
        if (loc != std::string::npos)
        {
            name = name.substr(loc, 3);
        }

        GUITools::DrawStringWithShadow(topLeft - Position(2, 3), name, white);
    }

    if (currentState.getCurrentFrame() < maxFinishTime)
    {
        GUITools::DrawLine(Position((PositionType)framePos, concurrent.y() - 10), Position((PositionType)framePos, concurrent.y() + (maxLayer)*(height + heightBuffer) + 10 + height), 1, white);
        GUITools::DrawStringWithShadow(Position((PositionType)framePos, concurrent.y() + (maxLayer)*(height + heightBuffer) + 10 + height + 20), stateTime.c_str(), white);
    }

    for (FrameCountType timeFrame = 0; timeFrame < maxFinishTime; timeFrame += 24*30)
    {
        float xPos = concurrent.x() + timeFrame*scale;
        GUITools::DrawStringWithShadow(Position((PositionType)xPos, concurrent.y() + (maxLayer)*(height + heightBuffer) + 10 + height + 20), getTimeString(timeFrame).c_str(), white);
    }

    Position progress = concurrent + Position(PositionType(maxWidth + 20), 0);
    Position progressBar(175,20);
    Position progressBuffer(0, 3);

    GUITools::DrawStringWithShadow(progress - Position(0,5), "Actions in Progress:", white);
    for (size_t a(0); a < currentState.getUnitData().getNumActionsInProgress(); ++a)
    {
        size_t index = currentState.getUnitData().getNumActionsInProgress() - a - 1;

        const ActionType actionInProgress = currentState.getUnitData().getActionInProgressByIndex(index);
        const FrameCountType finishTime = currentState.getUnitData().getActionInProgressFinishTimeByIndex(index);
        
        //DrawActionType(actionInProgress, progress, width);
        double remainingTime = (finishTime - currentState.getCurrentFrame());
        double timeRatio = remainingTime / actionInProgress.buildTime();
        Position ratioBar((PositionType)(timeRatio * progressBar.x()), progressBar.y());

        GUITools::DrawRect(progress + progressBuffer, progress + progressBuffer + progressBar, white);
        GUITools::DrawRect(progress + progressBuffer + Position(1,1), progress + progressBuffer - Position(1,1) + progressBar, grey);
        GUITools::DrawRect(progress + progressBuffer + Position(1,1), progress + progressBuffer - Position(1,1) + ratioBar, blue);

        std::stringstream time; time << actionInProgress.getName() << " " << (int)remainingTime;
        GUITools::DrawStringWithShadow(progress + progressBuffer + Position(10, 13), time.str(), white);
        progress.add(0, progressBar.y() + progressBuffer.y());
    }

    Position buildings(progress.x() + 195, concurrent.y());
    const BuildingData & buildingData = currentState.getBuildingData();
    GUITools::DrawStringWithShadow(buildings - Position(0,5), "Completed Buildings", white);
    for (size_t i(0); i < buildingData.size(); ++i)
    {
        const BuildingStatus & buildingStatus = buildingData.getBuilding(i);

        const ActionType & type = buildingStatus._type;
        const FrameCountType & finishTime = buildingStatus._timeRemaining;
        const ActionType & makingType = buildingStatus._isConstructing;

        GUITools::DrawRect(buildings + progressBuffer, buildings + progressBuffer + progressBar, white);
        GUITools::DrawRect(buildings + progressBuffer + Position(1,1), buildings + progressBuffer - Position(1,1) + progressBar, grey);

        if (finishTime > 0)
        {
            double remainingTime = finishTime;
            double timeRatio = remainingTime / makingType.buildTime();
            Position ratioBar((PositionType)(timeRatio * progressBar.x()), progressBar.y());
        
            GUITools::DrawRect(buildings + progressBuffer + Position(1,1), buildings + progressBuffer - Position(1,1) + ratioBar, blue);
            std::stringstream time; time << type.getName() << " " << (int)remainingTime;
            
            GUITools::DrawStringWithShadow(buildings + progressBuffer + Position(10, 13), time.str(), white);
        }
        else
        {
            GUITools::DrawStringWithShadow(buildings + progressBuffer + Position(10, 13), type.getName(), white);
        }
        
        buildings.add(0, progressBar.y() + progressBuffer.y());
    }

    return concurrent.y() + (maxLayer)*(height + heightBuffer) + 10 + height + 20 + 30;
}

void GUI::DrawActionType(const ActionType & type, const Position & topLeft, const size_t & width)
{
    GLfloat white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    int textureNumber = getTextureNumber(type);
    float ratio = (float)width / textureSizes[textureNumber].x();
    Position size = Position((PositionType)(textureSizes[textureNumber].x() * ratio), (PositionType)(textureSizes[textureNumber].y() * ratio));
    Position bottomRight(topLeft + size);
    GUITools::DrawTexturedRect(topLeft, bottomRight, textureNumber, white);
}

void GUI::AddActionTimes(const std::vector<FrameCountType> & startTimes, std::vector<FrameCountType> & finishTimes)
{
    _startTimes.push_back(startTimes);
    _finishTimes.push_back(finishTimes);
}

void GUI::AddBuildOrder(const std::vector<ActionType> & buildOrder, const size_t boIndex)
{
    Position bopos(0,0);

    _buildOrders.push_back(buildOrder);
    _boIndexes.push_back(boIndex);
}

void GUI::DrawAllUnits()
{
    GLfloat color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    float width = 64;
    Position p;
    Position techp(p.x(), p.y() + 100);

    for (size_t r(0); r < 3; ++r)
    {
        for (size_t i(0); i < ActionTypes::GetAllActionTypes(r).size(); ++i)
        {   
            const ActionType & type = ActionTypes::GetAllActionTypes(r)[i];
            DrawActionType(type, (type.isUnit() ? p : techp), (PositionType)width);
            
            if (type.isUnit())
            {
                p = p + Position((PositionType)width, 0);
            }
            else
            {
                techp = techp + Position((PositionType)width, 0);
            }
        }
        
        p = Position(0, (r+1) * 200);
        techp = Position(p.x(), p.y() + 100);
    }

}

void GUI::LoadTextures()
{
    textures = std::vector<GLuint>(BOSS_MAX_TEXTURES);
    textureSizes = std::vector<Position>(BOSS_MAX_TEXTURES);
    glGenTextures(BOSS_MAX_TEXTURES,&textures[0]);

    for (size_t r(0); r < Races::NUM_RACES; ++r)
    {
        for (size_t i(0); i < ActionTypes::GetAllActionTypes(r).size(); ++i)
        {
            const ActionType & type = ActionTypes::GetAllActionTypes(r)[i];

            LoadTexture(getTextureNumber(type), getTextureFileName(type).c_str(), false);
        }
    }
}

const int GUI::getTextureNumber(const ActionType & type) const
{
    return BOSS_TEXTURE_INTERVAL * type.getRace() + type.ID();
}

void GUI::LoadTexture(int textureNumber, const char * fileName, bool mipmap)
{
    struct stat buf;
    if (stat(fileName, &buf) == -1)
    {
        return;
    }

    SDL_Surface *surface;
    GLenum texture_format = GL_RGBA;
    GLint  nOfColors;

    if ( (surface = IMG_Load(fileName)) ) 
    { 
        // get the number of channels in the SDL surface
        nOfColors = surface->format->BytesPerPixel;
        if (nOfColors == 4)     // contains an alpha channel
        {
            //printf("Contains Alpha\n");
            if (surface->format->Rmask == 0x000000ff)
            {
                texture_format = GL_RGBA;
            }
        //else
        // texture_format = GL_BGRA;
        } 
        else if (nOfColors == 3)     // no alpha channel
        {
            if (surface->format->Rmask == 0x000000ff)
            {
                texture_format = GL_RGB;
            }
            //else
            // texture_format = GL_BGR;
        } 
        else 
        {
            printf("warning: %s is not truecolor..  this will probably break\n", fileName);
            // this error should not go unhandled
        }

        // Bind the texture object
        glBindTexture( GL_TEXTURE_2D, textureNumber );

		if (mipmap)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);
		}
    } 
    else 
    {
        printf("SDL could not load image: %s\n", SDL_GetError());
    }    

    // Free the SDL_Surface only if it was successfully created
    if ( surface ) 
    { 
        textureSizes[textureNumber] = Position(surface->w, surface->h);

        //printf("Loaded Image %s\n", fileName);
        SDL_FreeSurface( surface );
    }
}

void GUI::AddState(const GameState & state)
{
    _currentStates.push_back(state);
}

bool GUI::saveScreenshotBMP(const std::string & filename) 
{
    SDL_Surface * image = SDL_CreateRGBSurface(SDL_SWSURFACE, windowSizeX, windowSizeY, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);

    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, windowSizeX, windowSizeY, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

    SDL_Surface * imageFlipped = GUITools::flip_surface(image, GUITools::FLIP_VERTICAL);

    SDL_SaveBMP(imageFlipped, filename.c_str());
    SDL_FreeSurface(image);
    SDL_FreeSurface(imageFlipped);

    return true;
}

const std::string GUI::getTextureFileName(const ActionType & type) const
{
	std::string filename = "../starcraft_images/units/" + type.getName() + ".png";

    if (type.isTech())
    {
        filename = "../starcraft_images/command_icons/" + type.getName() + ".png";
    }
    else if (type.isUpgrade())
    {
        filename = "../starcraft_images/command_icons/" + type.getName() + ".png";
    }

	for (size_t i(0); i<filename.size(); ++i)
	{
		if (filename[i] == ' ')
		{
			filename[i] = '_';
		}
	}

	return filename;
}

#ifdef WIN32
    // disable vsync in windows
    #include "glext/wglext.h"
    #include "glext/glext.h"
    typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
    PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

void GUI::setVSync(int interval)
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