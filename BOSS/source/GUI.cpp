#include "GUI.h"

using namespace BOSS;

#define GUI_INITIAL_WIDTH  1280
#define GUI_INITIAL_HEIGHT 720

#define BOSS_MAX_TEXTURES 512
#define BOSS_TEXTURE_INTERVAL 64

const int GUI::TextureASCIIOffset           = 200;
const int GUI::TextureFont                  = 198;

GLfloat white[4] = {1.0f, 1.0f, 1.0f, 1.0f};

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
    , currentFrame(0)
    , lastFrameTime(0)
    , zoomX(1.0)
    , zoomY(1.0)
{
    timer.start();
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
    SDL_GL_SetSwapInterval(1);

#ifndef EMSCRIPTEN
    window = SDL_CreateWindow("Prismata OpenGL Visualization",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          windowSizeX, windowSizeY,
                          SDL_WINDOW_OPENGL);

    glcontext = SDL_GL_CreateContext(window);
#else
    screen = SDL_SetVideoMode(windowSizeX,windowSizeY,32,SDL_OPENGL | SDL_RESIZABLE);
#endif

    LoadTextures();

    // enable alpha blending for transparency
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glViewport(0,0,(GLsizei)(windowSizeX*zoomX),(GLsizei)(windowSizeY*zoomY));

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

    SDL_GL_SwapWindow(window);
}

void GUI::OnResize(SDL_Event & event)
{
    //windowSizeX = event.resize.w;
    //windowSizeY = event.resize.h;

    glViewport(0,0,(GLsizei)(windowSizeX*zoomX),(GLsizei)(windowSizeY*zoomY));
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
            /*case SDL_VIDEORESIZE:
            {
                OnResize(event);
                break;
            }*/
            case SDL_MOUSEMOTION:
            {
                if ((previousMouseX != 0 || previousMouseY != 0) && (event.motion.state & SDL_BUTTON_LEFT))
                {
                    cameraX -= (int)(event.motion.xrel * zoomX);
                    cameraY -= (int)(event.motion.yrel * zoomY);
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
            case SDL_MOUSEWHEEL:
            {
                //cameraY -= event.wheel.y;
                zoomX *= event.wheel.y < 0 ? 1.05 : 0.95;
                zoomY *= event.wheel.y < 0 ? 1.05 : 0.95;
                //glViewport(0,0,windowSizeX*zoomX,windowSizeY*zoomY);
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
				
			
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
            }
        }
    }
}

void GUI::Render()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    {
        glOrtho(0,windowSizeX*zoomX,windowSizeY*zoomY,0,-1,1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        {
            glTranslatef(static_cast<float>(-cameraX),static_cast<float>(-cameraY),0);
                
            _currentExperiment.onFrame();

            //GUITools::DrawTexturedRect(Position(0,0), Position(200,200), TextureFont, white);
        }

        glPopMatrix();
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    currentFrame++;
}

void GUI::DrawActionType(const ActionType & type, const Position & topLeft, const size_t & width)
{
    int textureNumber = getTextureNumber(type);
    float ratio = (float)width / textureSizes[textureNumber].x();
    Position size = Position((PositionType)(textureSizes[textureNumber].x() * ratio), (PositionType)(textureSizes[textureNumber].y() * ratio));
    Position bottomRight(topLeft + size);

    GUITools::DrawTexturedRect(topLeft, bottomRight, textureNumber, white);
}

int GUI::Width()
{
    return windowSizeX;
}

int GUI::Height()
{
    return windowSizeY;
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
    std::string imageDir = "../asset/images/";
#ifdef EMSCRIPTEN
    imageDir = "asset/images/";
#endif

    textures = std::vector<GLuint>(BOSS_MAX_TEXTURES);
    textureSizes = std::vector<Position>(BOSS_MAX_TEXTURES);
    glGenTextures(BOSS_MAX_TEXTURES,&textures[0]);

    for (size_t r(0); r < Races::NUM_RACES; ++r)
    {
        for (size_t i(0); i < ActionTypes::GetAllActionTypes(r).size(); ++i)
        {
            const ActionType & type = ActionTypes::GetAllActionTypes(r)[i];

            LoadTexture(getTextureNumber(type), imageDir + getTextureFileName(type).c_str());
        }
    }

    LoadTexture(TextureFont, imageDir + "fonts/alpha_trans.png");

}

const int GUI::getTextureNumber(const ActionType & type) const
{
    return BOSS_TEXTURE_INTERVAL * type.getRace() + type.ID();
}

void GUI::LoadTexture(int textureNumber, const std::string & fileName)
{
    struct stat buf;
    if (stat(fileName.c_str(), &buf) == -1)
    {
        std::cout << "Couldn't find texture: " << fileName << std::endl;
        return;
    }

    SDL_Surface *surface2 = IMG_Load(fileName.c_str());
    GLenum texture_format = GL_RGBA;
    GLint nOfColors = 4;

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
        textureSizes[textureNumber] = Position(surface2->w, surface2->h);
        SDL_FreeSurface( surface2 );
    }
}

void GUI::SetVisExperiment(BOSSVisExperiment & exp)
{
    _currentExperiment = exp;
}

bool GUI::saveScreenshotBMP(const std::string & filename) 
{
    SDL_Surface * image = SDL_CreateRGBSurface(SDL_SWSURFACE, windowSizeX, windowSizeY, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);

    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, windowSizeX, windowSizeY, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

    SDL_SaveBMP(image, filename.c_str());
    SDL_FreeSurface(image);

    return true;
}

const std::string GUI::getTextureFileName(const ActionType & type) const
{
	std::string filename = "units/" + type.getName() + ".png";

    if (type.isTech())
    {
        filename = "command_icons/" + type.getName() + ".png";
    }
    else if (type.isUpgrade())
    {
        filename = "command_icons/" + type.getName() + ".png";
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
