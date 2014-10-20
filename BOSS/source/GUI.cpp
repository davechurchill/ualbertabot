#include "GUI.h"

#ifdef BOSS_ENABLE_GUI

using namespace BOSS;

#define GUI_INITIAL_WIDTH  1600
#define GUI_INITIAL_HEIGHT 900

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

    OnFrame();
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
                DrawGameState();
            }

            glPopMatrix();
        }
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
    }
    glPopAttrib();
}

void GUI::DrawGameState()
{
    GLfloat color[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    Position p;

    for (size_t r(0); r < Races::NUM_RACES; ++r)
    {
        for (size_t i(0); i < ActionTypes::GetAllActionTypes(r).size(); ++i)
        {   
            const ActionType & type = ActionTypes::GetAllActionTypes(r)[i];

            Position br(p + textureSizes[getTextureNumber(type)]);

            GUITools::DrawTexturedRect(p, br, getTextureNumber(type), color);

            p = p + Position(textureSizes[getTextureNumber(type)].x(), 0);
        }
        
        p = Position(0, (r+1) * 150);
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
        filename = "../starcraft_images/command_icons/" + type.getTechType().getRace().getName() + "_" + type.getName() + ".png";
    }
    else if (type.isUpgrade())
    {
        filename = "../starcraft_images/command_icons/" + type.getUpgradeType().getRace().getName() + "_" + type.getName() + ".png";
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

#endif