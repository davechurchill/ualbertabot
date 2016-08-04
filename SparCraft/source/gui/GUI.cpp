#include "GUI.h"
#include "BWAPI.h"
#include <cassert>
#include <iostream>

using namespace SparCraft;

const size_t MaxStarCraftTextures                   = 512;
const int GUI::TextureFont                 = 256;

GLfloat ColorWhite[4] = {1.0f, 1.0f, 1.0f, 1.0f};

GUI::GUI(int width, int height) 
    : _initialWidth(width)
    , _initialHeight(height)
    , _cameraX(0)
    , _cameraY(0)
    , _previousMouseX(0)
    , _previousMouseY(0)
    , _isStarted(false)
    , _mousePressed(false)
    , _shiftPressed(false)
    , _currentFrame(0)
    , _previousRenderTime(0)
    , _guiGame(*this)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << "Could not initialize SDL\n";
        exit(-1);
    }

    onStart();
}

GUI::~GUI()
{
    SDL_Quit();
}

bool GUI::isStarted() const
{
    return _isStarted;
}

// This function must be called before OnFrame
void GUI::onStart()
{
    // if we've already called OnStart, don't re-initialize everything
    if (_isStarted)
    {
        return;
    }

    // the top-left corner of the scene will be 0, 0
    _cameraX = 0;
    _cameraY = 0;

    // double buffer and swap attributes, makes switching scenes fast
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_GL_SetSwapInterval(1);

    // set up the window that the OpenGL context will be bound to
    _window = SDL_CreateWindow("StarCraft OpenGL Visualization", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _initialWidth, _initialHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    // set the glcontext to the window we just created
    _glcontext = SDL_GL_CreateContext(_window);

    // load all the Starcraft textures that we'll need
    loadTextures();

    // enable alpha blending for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // set up the viewport
    glViewport(0, 0, width(), height());

    _isStarted = true;
}

void GUI::onFrame()
{
    SPARCRAFT_ASSERT(isStarted(), "Must initialize GUI before calling OnFrame()");

    // Handle input events
    handleEvents();

    // Render the frame
    glClear(GL_COLOR_BUFFER_BIT);
    render();

    SDL_GL_SwapWindow(_window);
}

void GUI::handleEvents()
{
    // Handle SDL events
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        const bool pressed(event.key.state == SDL_PRESSED);
        switch (event.type)
        {
            case SDL_MOUSEMOTION:
            {
                if ((_previousMouseX != 0 || _previousMouseY != 0) && (event.motion.state & SDL_BUTTON_LEFT))
                {
                    _cameraX -= event.motion.xrel;
                    _cameraY -= event.motion.yrel;
                }

                _previousMouseX = event.motion.x;
                _previousMouseY = event.motion.y;
                break;
            }
            case SDL_KEYDOWN:
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_LSHIFT:
                    _shiftPressed = pressed;
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
                    _shiftPressed = pressed;
                    break;
                }
                break;
            }
            case SDL_MOUSEWHEEL:
            {

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
            case SDL_WINDOWEVENT_RESIZED:
            {
            
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

void GUI::render()
{
    Timer renderTimer;
    renderTimer.start();

    glViewport(0, 0, width(), height());

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    {
        glOrtho(0, width(), height(), 0, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        {
            glTranslatef(static_cast<float>(-_cameraX),static_cast<float>(-_cameraY),0);
              
            _guiGame.onFrame();

            //drawAllBWAPIUnits();
            
            //GUITools::DrawTexturedRect(Position(0,0), Position(200,200), TextureFont, ColorWhite);
            //GUITools::DrawString(Position(300, 300), "Test String", ColorWhite);
        }

        glPopMatrix();
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    _currentFrame++;
}

int GUI::width()
{
    int x, y;
    SDL_GetWindowSize(_window, &x, &y);

    return x;
}

int GUI::height()
{
    int x, y;
    SDL_GetWindowSize(_window, &x, &y);

    return y;
}

void GUI::setCenter(int x, int y)
{
    _cameraX = -(width() - x) / 2;
    _cameraY = -(height() - y) / 2;
}

void GUI::drawAllBWAPIUnits()
{
    Position p(0, 0);
    size_t maxHeight = 0;

    std::vector<int> allIDs;

    for (const auto & kv : _techTypeTextureID)
    {
        allIDs.push_back(kv.second);
    }

    for (const auto & kv : _upgradeTypeTextureID)
    {
        allIDs.push_back(kv.second);
    }

    for (const auto & kv : _unitTypeTextureID)
    {
        allIDs.push_back(kv.second);
    }

    for (const auto & id : allIDs)
    {
        if (p.x() + _textureSizes[id].x() > width())
        {
            p = Position(0, p.y() + maxHeight);
            maxHeight = 0;
        }

        GUITools::DrawTexturedRect(p, p + _textureSizes[id], id, ColorWhite);
            
        maxHeight = std::max((size_t)_textureSizes[id].y(), maxHeight);
            
        p = p + (Position(_textureSizes[id].x(), 0));
    }
}

void GUI::drawUnitType(const BWAPI::UnitType & type, const Position & p)
{
    const int id = _unitTypeTextureID[type];
    //GUITools::DrawString(p, type.getName(), ColorWhite);
    Position pos = p - Position(_textureSizes[id].x()/2, _textureSizes[id].y()/2);

    GUITools::DrawTexturedRect(pos, pos + _textureSizes[id], id, ColorWhite);
}

void GUI::loadTextures()
{
    std::string imageDir = "../asset/images/";

    // set up the vectors that will hold the textures
    _textures = std::vector<GLuint>(MaxStarCraftTextures, 0);
    _textureSizes = std::vector<Position>(MaxStarCraftTextures);
    glGenTextures(MaxStarCraftTextures, &_textures[0]);

    // load all the starcraft unit textures
    size_t textureNumber = 1;
    for (const BWAPI::UnitType & type : BWAPI::UnitTypes::allUnitTypes())
    {
        if (loadTexture(textureNumber, imageDir + GetTextureFileName(type)))
        {
            _unitTypeTextureID[type] = textureNumber;
            textureNumber++;
        }
    }

    for (const BWAPI::TechType & type : BWAPI::TechTypes::allTechTypes())
    {
        if (loadTexture(textureNumber, imageDir + GetTextureFileName(type)))
        {
            _techTypeTextureID[type] = textureNumber;
            textureNumber++;
        }
    }

    for (const BWAPI::UpgradeType & type : BWAPI::UpgradeTypes::allUpgradeTypes())
    {
        if (loadTexture(textureNumber, imageDir + GetTextureFileName(type)))
        {
            _upgradeTypeTextureID[type] = textureNumber;
            textureNumber++;
        }
    }
    
    loadTexture(TextureFont, imageDir + "fonts/alpha_trans.png");

    std::cout << "\n\nSuccessfully loaded " << textureNumber << " textures\n\n";
}

bool GUI::loadTexture(int textureNumber, const std::string & fileName)
{
    struct stat buf;
    if (stat(fileName.c_str(), &buf) == -1)
    {
        //std::cout << "Couldn't find texture: " << fileName << std::endl;
        return false;
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

    if (surface2) 
    { 
        _textureSizes[textureNumber] = Position(surface2->w, surface2->h);
        SDL_FreeSurface( surface2 );
    }

    //std::cout << textureNumber << "Loaded: " << fileName << std::endl;

    return true;
}

bool GUI::saveScreenshotBMP(const std::string & filename) 
{
    SDL_Surface * image = SDL_CreateRGBSurface(SDL_SWSURFACE, width(), height(), 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);

    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, width(), height(), GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

    SDL_SaveBMP(image, filename.c_str());
    SDL_FreeSurface(image);

    return true;
}

std::string GUI::GetTextureFileName(const BWAPI::TechType & type)
{
	std::string filename = "command_icons/" + type.getName() + ".png";

	for (size_t i(0); i < filename.size(); ++i)
	{
		if (filename[i] == ' ')
		{
			filename[i] = '_';
		}
	}

	return filename;
}

std::string GUI::GetTextureFileName(const BWAPI::UnitType & type)
{
	std::string filename = "units/" + type.getName() + ".png";

	for (size_t i(0); i < filename.size(); ++i)
	{
		if (filename[i] == ' ')
		{
			filename[i] = '_';
		}
	}

	return filename;
}

std::string GUI::GetTextureFileName(const BWAPI::UpgradeType & type)
{
	std::string filename = "command_icons/" + type.getName() + ".png";

	for (size_t i(0); i < filename.size(); ++i)
	{
		if (filename[i] == ' ')
		{
			filename[i] = '_';
		}
	}

	return filename;
}

void GUI::setGame(const Game & game)
{
    _guiGame.setGame(game);
}

const Game & GUI::getGame() const
{
    return _guiGame.getGame();
}
